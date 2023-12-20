#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "EnvMapAsset.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT 32

#define POINT_SHADOW_RESOLUTION 2048

namespace Hanabi
{
	struct SceneRendererData
	{
		struct DirectionalLight
		{
			glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
			float Intensity = 0.0f;
			glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };

			// Padding
			char padding[4];
		};

		struct PointLight
		{
			glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
			float Intensity = 0.0f;
			glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
			float Radius;
			float Falloff;

			// Padding
			char padding[12];
		};

		struct SpotLight
		{
			glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
			float Intensity = 0.0f;
			glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
			float AngleAttenuation;
			glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
			float Range;
			float Angle;
			float Falloff;

			// Padding
			char padding[8];
		};

		struct MaterialData
		{
			glm::vec3 Albedo;
			float Emission;
			float Metalness;
			float Roughness;
			bool UseNormalMap;

			// Padding
			// Notice : bool is 1 byte in C++ but 4 bytes in HLSL
			char padding[7];
		};

		struct CBModel
		{
			glm::mat4 Transform;
			MaterialData Material;
		};

		struct CBCamera
		{
			glm::mat4 ViewProj;
			glm::vec3 CameraPosition;

			// Padding
			char padding[4];
		};

		struct CBScene
		{
			float SkyLightIntensity = 0.0f;

			// Padding
			char padding[12];

			DirectionalLight Light;
		};

		struct CBPointLight
		{
			uint32_t Count = 0;

			// Padding
			char padding[12];

			PointLight PointLights[MAX_POINT_LIGHT]{};
		};

		struct CBSpotLight
		{
			uint32_t Count = 0;

			// Padding
			char padding[12];

			SpotLight SpotLights[MAX_SPOT_LIGHT]{};
		};

		struct CBDirShadow
		{
			glm::mat4 LightViewProj;
			uint32_t ShadowType = 0;

			// Padding
			char padding[12];
		};

		struct CBPointShadow
		{
			glm::mat4 LightViewProj[6];
			glm::vec3 LightPosition;
			float FarPlane = 0.0f;
			uint32_t ShadowType = 0;

			// Padding
			char padding[12];
		};

		Ref<Environment> SceneEnvironment;

		CBModel ModelData;
		CBCamera CameraData;
		CBScene SceneData;
		CBPointLight PointLightData;
		CBSpotLight SpotLightData;
		CBDirShadow DirShadowData;
		CBPointShadow PointShadowData;

		Ref<ConstantBuffer> ModelDataBuffer;
		Ref<ConstantBuffer> CameraDataBuffer;
		Ref<ConstantBuffer> SceneDataBuffer;
		Ref<ConstantBuffer> PointLightDataBuffer;
		Ref<ConstantBuffer> SpotLightDataBuffer;
		Ref<ConstantBuffer> DirShadowDataBuffer;
		Ref<ConstantBuffer> PointShadowDataBuffer;

		Ref<RenderPass> DirShadowMapPass;
		Ref<RenderPass> PointShadowMapPass;
		Ref<RenderPass> DeferredGeoPass;
		Ref<RenderPass> DeferredLightingPass;
		Ref<RenderPass> SkyboxPass;
		Ref<RenderPass> CompositePass;

		Ref<MaterialAsset> DefaultMaterialAsset;

		struct GeoDrawCommand
		{
			Ref<Mesh> Mesh;
			uint32_t SubmeshIndex;
			Ref<Material> Material;

			CBModel ModelData;
		};
		std::vector<GeoDrawCommand> DrawCommands;
		SceneRenderer::Statistics RendererStats;
	};
	static SceneRendererData* s_Data;

	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData();

		s_Data->ModelDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBModel));
		s_Data->CameraDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBCamera));
		s_Data->SceneDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBScene));
		s_Data->PointLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBPointLight));
		s_Data->SpotLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBSpotLight));
		s_Data->DirShadowDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBDirShadow));
		s_Data->PointShadowDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBPointShadow));

		VertexBufferLayout GeoLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		};

		VertexBufferLayout FullScreenQuadLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		};

		VertexBufferLayout SkyboxLayout = {
			{ ShaderDataType::Float3, "a_Position" },
		};

		// Deferred Geometry Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA32F, ImageFormat::RGBA16F,
					ImageFormat::RGBA16F,ImageFormat::RGBA16F,ImageFormat::Depth };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = GeoLayout;
				pipelineSpec.Shader = Renderer::GetShader("DeferredGeometry");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->DeferredGeoPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Dir Shadow Map Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::ShadowMap };
				spec.Width = 2048;
				spec.Height = 2048;
				spec.SwapChainTarget = false;
				spec.DepthClearValue = 1.0f;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				// Notice: Use the same layout as DeferredGeoPass, 
				// because transparent objects need check alpha value from Albedo Texture
				pipelineSpec.Layout = GeoLayout;
				pipelineSpec.Shader = Renderer::GetShader("DirShadowMap");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->DirShadowMapPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Point Shadow Map Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { { ImageFormat::ShadowMap,6 } };
				spec.Width = POINT_SHADOW_RESOLUTION;
				spec.Height = POINT_SHADOW_RESOLUTION;
				spec.SwapChainTarget = false;
				spec.DepthClearValue = 1.0f;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				// Notice: Use the same layout as DeferredGeoPass, 
				// because transparent objects need check alpha value from Albedo Texture
				pipelineSpec.Layout = GeoLayout;
				pipelineSpec.Shader = Renderer::GetShader("PointShadowMap");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->PointShadowMapPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Deferred Lighting Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA32F };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = FullScreenQuadLayout;
				pipelineSpec.Shader = Renderer::GetShader("DeferredLighting");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = false;
				pipelineSpec.DepthTest = false;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->DeferredLightingPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Composite Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA32F };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = FullScreenQuadLayout;
				pipelineSpec.Shader = Renderer::GetShader("Composite");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = false;
				pipelineSpec.DepthTest = false;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->CompositePass = RenderPass::Create(renderPassSpec);
			}
		}
		// Skybox Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA32F,ImageFormat::Depth };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				spec.ExistingImages[0] = s_Data->CompositePass->GetOutput(0);
				spec.ExistingImages[1] = s_Data->DeferredGeoPass->GetDepthOutput();
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = SkyboxLayout;
				pipelineSpec.Shader = Renderer::GetShader("Skybox");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = false;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::LessEqual;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->SkyboxPass = RenderPass::Create(renderPassSpec);
			}
		}

		s_Data->DeferredGeoPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->DeferredGeoPass->SetInput("CBCamera", s_Data->CameraDataBuffer);

		s_Data->DirShadowMapPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->DirShadowMapPass->SetInput("CBDirShadow", s_Data->DirShadowDataBuffer);
		s_Data->PointShadowMapPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->PointShadowMapPass->SetInput("CBPointShadow", s_Data->PointShadowDataBuffer);

		s_Data->DeferredLightingPass->SetInput("CBCamera", s_Data->CameraDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBScene", s_Data->SceneDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBPointLight", s_Data->PointLightDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBSpotLight", s_Data->SpotLightDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBDirShadow", s_Data->DirShadowDataBuffer);
		s_Data->DeferredLightingPass->SetInput("u_AlbedoBuffer", s_Data->DeferredGeoPass->GetOutput(0));
		s_Data->DeferredLightingPass->SetInput("u_MREBuffer", s_Data->DeferredGeoPass->GetOutput(1));
		s_Data->DeferredLightingPass->SetInput("u_NormalBuffer", s_Data->DeferredGeoPass->GetOutput(2));
		s_Data->DeferredLightingPass->SetInput("u_PositionBuffer", s_Data->DeferredGeoPass->GetOutput(3));
		s_Data->DeferredLightingPass->SetInput("u_DirShadowMap", s_Data->DirShadowMapPass->GetDepthOutput());
		s_Data->DeferredLightingPass->SetInput("u_PointShadowMap", s_Data->PointShadowMapPass->GetDepthOutput());
		s_Data->DeferredLightingPass->SetInput("u_BRDFLUTTex", Renderer::GetTexture<Texture2D>("BRDFLut"));

		s_Data->CompositePass->SetInput("u_Color", s_Data->DeferredLightingPass->GetOutput());

		s_Data->SkyboxPass->SetInput("CBCamera", s_Data->CameraDataBuffer);

		s_Data->DefaultMaterialAsset = CreateRef<MaterialAsset>();
	}

	void SceneRenderer::Shutdown()
	{
		delete s_Data;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		const FramebufferSpecification& fbsc = s_Data->CompositePass->GetTargetFramebuffer()->GetSpecification();
		if (fbsc.Width != width || fbsc.Height != height)
		{
			// TODO: Resize color attachment and depth attachment
			/*	s_Data->DeferredGeoPass->GetTargetFramebuffer()->Resize(width, height);
				s_Data->DeferredLightingPass->GetTargetFramebuffer()->Resize(width, height);
				s_Data->CompositePass->GetTargetFramebuffer()->Resize(width, height);

				{
					Ref<Framebuffer> framebuffer;
					{
						FramebufferSpecification spec;
						spec.Attachments = { ImageFormat::RGBA32F,ImageFormat::Depth };
						spec.Width = width;
						spec.Height = height;
						spec.SwapChainTarget = false;
						spec.ExistingImages[0] = s_Data->CompositePass->GetOutput(0);
						spec.ExistingImages[1] = s_Data->DeferredGeoPass->GetDepthOutput();
						framebuffer = Framebuffer::Create(spec);
					}
					s_Data->SkyboxPass->GetPipeline()->GetSpecification().TargetFramebuffer = framebuffer;
				}*/
		}
	}

	void SceneRenderer::BeginScene(const Ref<Environment> environment)
	{
		ResetStats();

		s_Data->CameraData.ViewProj = environment->ViewProjection;
		s_Data->CameraData.CameraPosition = environment->CameraPosition;
		s_Data->CameraDataBuffer->SetData(&s_Data->CameraData);

		s_Data->SceneEnvironment = environment;

		// Sky Light
		{
			s_Data->SceneData.SkyLightIntensity = environment->SkyLightIntensity;

			if (environment->EnvMapHandle)
			{
				Ref<EnvMapAsset> asset = AssetManager::GetAsset<EnvMapAsset>(environment->EnvMapHandle);
				s_Data->SkyboxPass->SetInput("u_RadianceMap", asset->RadianceMap);
				s_Data->DeferredLightingPass->SetInput("u_EnvRadianceTex", asset->RadianceMap);
				s_Data->DeferredLightingPass->SetInput("u_EnvIrradianceTex", asset->IrradianceMap);

				// Debug
				//Ref<Texture2D> equirectangularMap = AssetManager::GetAsset<Texture2D>(environment->EnvMapHandle);
				//auto [radiance, irradiance] = Renderer::CreateEnvironmentMap(equirectangularMap);
				//s_Data->SkyboxPass->SetInput("u_RadianceMap", radiance);
			}
			else
			{
				s_Data->SkyboxPass->SetInput("u_RadianceMap", Renderer::GetTexture<TextureCube>("BlackCube"));
				s_Data->DeferredLightingPass->SetInput("u_EnvRadianceTex", Renderer::GetTexture<TextureCube>("BlackCube"));
				s_Data->DeferredLightingPass->SetInput("u_EnvIrradianceTex", Renderer::GetTexture<TextureCube>("BlackCube"));
			}
		}

		// Directional Light
		{
			s_Data->SceneData.Light.Direction = environment->DirLight.Direction;
			s_Data->SceneData.Light.Intensity = environment->DirLight.Intensity;
			s_Data->SceneData.Light.Radiance = environment->DirLight.Radiance;
			s_Data->SceneDataBuffer->SetData(&s_Data->SceneData);
		}

		// Point Lights
		{
			s_Data->PointLightData.Count = environment->PointLights.size();
			for (uint32_t i = 0; i < s_Data->PointLightData.Count; i++)
			{
				s_Data->PointLightData.PointLights[i] = {
					environment->PointLights[i].Position,
					environment->PointLights[i].Intensity,
					environment->PointLights[i].Radiance,
					environment->PointLights[i].Radius,
					environment->PointLights[i].Falloff,
				};
			}
			s_Data->PointLightDataBuffer->SetData(&s_Data->PointLightData);
		}

		// Spot Lights
		{
			s_Data->SpotLightData.Count = environment->SpotLights.size();
			for (uint32_t i = 0; i < s_Data->SpotLightData.Count; i++)
			{
				s_Data->SpotLightData.SpotLights[i] = {
					environment->SpotLights[i].Position,
					environment->SpotLights[i].Intensity,
					environment->SpotLights[i].Radiance,
					environment->SpotLights[i].AngleAttenuation,
					environment->SpotLights[i].Direction,
					environment->SpotLights[i].Range,
					environment->SpotLights[i].Angle,
					environment->SpotLights[i].Falloff,
				};
			}
			s_Data->SpotLightDataBuffer->SetData(&s_Data->SpotLightData);
		}

		s_Data->DrawCommands.clear();
	}

	void SceneRenderer::EndScene()
	{
		DeferredGeoPass();

		DirShadowMapPass();

		DeferredLightPass();

		CompositePass();
		SkyboxPass();
	}

	Ref<RenderPass> SceneRenderer::GetFinalPass()
	{
		return s_Data->SkyboxPass;
	}

	Ref<Image2D> SceneRenderer::GetGBufferAlbedo()
	{
		return s_Data->DeferredGeoPass->GetOutput(0);
	}

	Ref<Image2D> SceneRenderer::GetGBufferMRE()
	{
		return s_Data->DeferredGeoPass->GetOutput(1);
	}

	Ref<Image2D> SceneRenderer::GetGBufferNormal()
	{
		return s_Data->DeferredGeoPass->GetOutput(2);
	}

	Ref<Image2D> SceneRenderer::GetGBufferPosition()
	{
		return s_Data->DeferredGeoPass->GetOutput(3);
	}

	Ref<Image2D> SceneRenderer::GetDirShadowMap()
	{
		return s_Data->DirShadowMapPass->GetDepthOutput();
	}

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, MeshComponent& meshComponent, AssetHandle overrideMaterialHandle)
	{
		if (meshComponent.MeshSourceHandle)
		{
			if (!AssetManager::IsAssetHandleValid(meshComponent.MeshHandle))
			{
				Ref<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>(meshComponent.MeshSourceHandle);
				meshComponent.MeshHandle = AssetManager::CreateMemoryOnlyAsset<Mesh>(meshSource);
			}

			Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshComponent.MeshHandle);
			Ref<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>(meshComponent.MeshSourceHandle);
			Ref<MaterialAsset> materialAsset = s_Data->DefaultMaterialAsset;

			if (AssetManager::IsAssetHandleValid(overrideMaterialHandle))
			{
				materialAsset = AssetManager::GetAsset<MaterialAsset>(overrideMaterialHandle);
			}
			else if (meshSource->GetMaterials().size() > 0)
			{
				AssetHandle materialHandle = meshSource->GetMaterials()[meshSource->GetSubmeshes()[meshComponent.SubmeshIndex].MaterialIndex];
				if (AssetManager::IsAssetHandleValid(materialHandle))
				{
					materialAsset = AssetManager::GetAsset<MaterialAsset>(materialHandle);
				}
			}

			s_Data->DrawCommands.push_back({
					mesh,
					meshComponent.SubmeshIndex,
					materialAsset->GetMaterial(),
					{
						transform,
						materialAsset->GetAlbedo(),
						materialAsset->GetEmission(),
						materialAsset->GetMetalness(),
						materialAsset->GetRoughness(),
						materialAsset->IsUsingNormalMap(),
					}
				});
		}
	}

	void SceneRenderer::ResetStats()
	{
		s_Data->RendererStats = {};
	}

	SceneRenderer::Statistics SceneRenderer::GetStats()
	{
		return s_Data->RendererStats;
	}

	void SceneRenderer::ExecuteDrawCommands()
	{
		for (auto& command : s_Data->DrawCommands)
		{
			s_Data->ModelData.Transform = command.ModelData.Transform;
			s_Data->ModelData.Material = command.ModelData.Material;

			s_Data->ModelDataBuffer->SetData(&s_Data->ModelData);

			s_Data->RendererStats.DrawCalls++;

			Renderer::DrawMesh(command.Mesh, command.SubmeshIndex, command.Material);
		}
	}

	void SceneRenderer::DeferredGeoPass()
	{
		Renderer::BeginRenderPass(s_Data->DeferredGeoPass);
		ExecuteDrawCommands();
		Renderer::EndRenderPass();
	}

	void SceneRenderer::DeferredLightPass()
	{
		Renderer::BeginRenderPass(s_Data->DeferredLightingPass);
		Renderer::DrawFullScreenQuad();
		Renderer::EndRenderPass();
	}

	void SceneRenderer::DirShadowMapPass()
	{
		// Directional Light
		{
			Renderer::BeginRenderPass(s_Data->DirShadowMapPass);
			s_Data->DirShadowData.ShadowType = static_cast<uint32_t>(s_Data->SceneEnvironment->DirLight.ShadowType);
			if (s_Data->SceneEnvironment->DirLight.ShadowType != LightComponent::ShadowType::None)
			{
				glm::vec3 lightPosition = glm::vec3(0.0f) - (s_Data->SceneEnvironment->DirLight.Direction * 200.0f);
				glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 lightOrthoMatrix = glm::ortho(-350.0f, 350.0f, -350.0f, 350.0f, 0.1f, 750.0f);
				s_Data->DirShadowData.LightViewProj = lightOrthoMatrix * lightViewMatrix;

				s_Data->DirShadowDataBuffer->SetData(&s_Data->DirShadowData);

				ExecuteDrawCommands();
			}
			else
			{
				s_Data->DirShadowDataBuffer->SetData(&s_Data->DirShadowData);
			}
			Renderer::EndRenderPass();
		}

		// Point Light Shadow
		{
			Renderer::BeginRenderPass(s_Data->PointShadowMapPass);
			// TODO: Only support one point light now
			s_Data->PointShadowData.ShadowType = static_cast<uint32_t>(s_Data->SceneEnvironment->PointLights[0].ShadowType);
			if (s_Data->SceneEnvironment->PointLights[0].ShadowType != LightComponent::ShadowType::None)
			{
				s_Data->PointShadowData.LightPosition = s_Data->SceneEnvironment->PointLights[0].Position;
				s_Data->PointShadowData.FarPlane = s_Data->SceneEnvironment->PointLights[0].Radius;

				static float s_PointShadowAspect = (float)POINT_SHADOW_RESOLUTION / (float)POINT_SHADOW_RESOLUTION;
				const static float s_PointShadowNearPlane = 1.0f;
				glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), s_PointShadowAspect,
					s_PointShadowNearPlane, s_Data->PointShadowData.FarPlane);

				s_Data->PointShadowData.LightViewProj[0] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(1.0, 0.0, 0.0),
						glm::vec3(0.0, 1.0, 0.0));
				s_Data->PointShadowData.LightViewProj[1] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(-1.0, 0.0, 0.0),
						glm::vec3(0.0, 1.0, 0.0));
				s_Data->PointShadowData.LightViewProj[2] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(0.0, 1.0, 0.0),
						glm::vec3(0.0, 0.0, -1.0));
				s_Data->PointShadowData.LightViewProj[3] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(0.0, -1.0, 0.0),
						glm::vec3(0.0, 0.0, 1.0));
				s_Data->PointShadowData.LightViewProj[4] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(0.0, 0.0, 1.0),
						glm::vec3(0.0, 1.0, 0.0));
				s_Data->PointShadowData.LightViewProj[5] = shadowProj *
					glm::lookAt(s_Data->PointShadowData.LightPosition, s_Data->PointShadowData.LightPosition + glm::vec3(0.0, 0.0, -1.0),
						glm::vec3(0.0, 1.0, 0.0));

				s_Data->PointShadowDataBuffer->SetData(&s_Data->PointShadowData);

				ExecuteDrawCommands();
			}
			else
			{
				s_Data->DirShadowDataBuffer->SetData(&s_Data->DirShadowData);
			}
			Renderer::EndRenderPass();
		}

		// TODO: Implement Spot Light Shadow
	}

	void SceneRenderer::CompositePass()
	{
		Renderer::BeginRenderPass(s_Data->CompositePass);
		Renderer::DrawFullScreenQuad();
		Renderer::EndRenderPass();
	}

	void SceneRenderer::SkyboxPass()
	{
		Renderer::BeginRenderPass(s_Data->SkyboxPass, false);
		Ref<Mesh> mesh = Renderer::GetMesh("Box");
		Renderer::DrawMesh(mesh);
		Renderer::EndRenderPass();
	}

	void SceneRenderer::CalculateCascades(CascadeData* cascades)
	{
		float scaleToOrigin = 0.0f;
		float CascadeSplitLambda = 0.92f;
		float CascadeFarPlaneOffset = 50.0f, CascadeNearPlaneOffset = -50.0f;

		glm::mat4 viewMatrix = s_Data->SceneEnvironment->View;
		constexpr glm::vec4 origin = glm::vec4(glm::vec3(0.0f), 1.0f);
		viewMatrix[3] = glm::lerp(viewMatrix[3], origin, scaleToOrigin);

		auto viewProjection = s_Data->SceneEnvironment->Projection * viewMatrix;

		const int SHADOW_MAP_CASCADE_COUNT = 4;
		float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

		float nearClip = s_Data->SceneEnvironment->CameraNearClip;
		float farClip = s_Data->SceneEnvironment->CameraFarClip;
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		// Calculate split depths based on view camera frustum
		// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = CascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearClip) / clipRange;
		}

		cascadeSplits[3] = 0.3f;

		// Manually set cascades here
		// cascadeSplits[0] = 0.05f;
		// cascadeSplits[1] = 0.15f;
		// cascadeSplits[2] = 0.3f;
		// cascadeSplits[3] = 1.0f;

		// Calculate orthographic projection matrix for each cascade
		float lastSplitDist = 0.0;
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float splitDist = cascadeSplits[i];

			glm::vec3 frustumCorners[8] =
			{
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
			};

			// Project frustum corners into world space
			glm::mat4 invCam = glm::inverse(viewProjection);
			for (uint32_t i = 0; i < 8; i++)
			{
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = invCorner / invCorner.w;
			}

			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}

			// Get frustum center
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++)
				frustumCenter += frustumCorners[i];

			frustumCenter /= 8.0f;

			//frustumCenter *= 0.01f;

			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = -s_Data->SceneEnvironment->DirLight.Direction;
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f + CascadeNearPlaneOffset, maxExtents.z - minExtents.z + CascadeFarPlaneOffset);

			// Offset to texel space to avoid shimmering (from https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering)
			glm::mat4 shadowMatrix = lightOrthoMatrix * lightViewMatrix;
			float ShadowMapResolution = (float)s_Data->DirShadowMapPass->GetTargetFramebuffer()->GetWidth();

			glm::vec4 shadowOrigin = (shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) * ShadowMapResolution / 2.0f;
			glm::vec4 roundedOrigin = glm::round(shadowOrigin);
			glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
			roundOffset = roundOffset * 2.0f / ShadowMapResolution;
			roundOffset.z = 0.0f;
			roundOffset.w = 0.0f;

			lightOrthoMatrix[3] += roundOffset;

			// Store split distance and matrix in cascade
			cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
			cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
			cascades[i].View = lightViewMatrix;

			lastSplitDist = cascadeSplits[i];
		}
	}
}