#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "EnvMapAsset.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT 32

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

		struct CBShadow
		{
			glm::mat4 LightViewProj;
			uint32_t ShadowType = 0;
			uint32_t LightType = 0;

			// Padding
			char padding[8];
		};

		Ref<Environment> SceneEnvironment;

		CBModel ModelData;
		CBCamera CameraData;
		CBScene SceneData;
		CBPointLight PointLightData;
		CBSpotLight SpotLightData;
		CBShadow ShadowData;

		Ref<ConstantBuffer> ModelDataBuffer;
		Ref<ConstantBuffer> CameraDataBuffer;
		Ref<ConstantBuffer> SceneDataBuffer;
		Ref<ConstantBuffer> PointLightDataBuffer;
		Ref<ConstantBuffer> SpotLightDataBuffer;
		Ref<ConstantBuffer> ShadowDataBuffer;

		Ref<RenderPass> ShadowMapPass;
		Ref<RenderPass> ShadowMappingPass;
		Ref<RenderPass> DeferredGeoPass;
		Ref<RenderPass> DeferredLightingPass;
		Ref<RenderPass> SkyboxPass;
		Ref<RenderPass> CompositePass;

		Ref<Material> DefaultMaterial;

		struct GeoDrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<Material> Material;

			CBModel ModelData;
		};
		std::vector<GeoDrawCommand> DrawCommands;
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
		s_Data->ShadowDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBShadow));

		// Deferred Geometry Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA8,ImageFormat::RGBA8,
					ImageFormat::RGBA16F,ImageFormat::RGBA16F,ImageFormat::Depth };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				VertexBufferLayout vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float3, "a_Normal" },
					{ ShaderDataType::Float3, "a_Tangent" },
					{ ShaderDataType::Float3, "a_Bitangent" },
					{ ShaderDataType::Float2, "a_TexCoord" },
				};

				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = vertexLayout;
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
		// Deferred Lighting Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA8 };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = {
				   { ShaderDataType::Float3, "a_Position" },
				   { ShaderDataType::Float2, "a_TexCoord" },
				};
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
		// Shadow Map Pass
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
				VertexBufferLayout vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
				};

				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = vertexLayout;
				pipelineSpec.Shader = Renderer::GetShader("ShadowMap");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->ShadowMapPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Shadow Mapping Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA8 };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				framebuffer = Framebuffer::Create(spec);
			}
			{
				VertexBufferLayout vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoord" },
				};

				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = vertexLayout;
				pipelineSpec.Shader = Renderer::GetShader("ShadowMapping");
				pipelineSpec.TargetFramebuffer = framebuffer;
				pipelineSpec.BackfaceCulling = false;
				pipelineSpec.DepthTest = false;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->ShadowMappingPass = RenderPass::Create(renderPassSpec);
			}
		}
		// Composite Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				{
					FramebufferSpecification spec;
					spec.Attachments = { ImageFormat::RGBA8 };
					spec.Width = 1920;
					spec.Height = 1080;
					spec.SwapChainTarget = false;
					framebuffer = Framebuffer::Create(spec);
				}
				{
					PipelineSpecification pipelineSpec;
					pipelineSpec.Layout = {
					   { ShaderDataType::Float3, "a_Position" },
					   { ShaderDataType::Float2, "a_TexCoord" },
					};
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
		}
		// Skybox Pass
		{
			Ref<Framebuffer> framebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA8,ImageFormat::Depth };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				spec.ExistingImages[0] = s_Data->CompositePass->GetOutput(0);
				spec.ExistingImages[1] = s_Data->DeferredGeoPass->GetDepthOutput();
				framebuffer = Framebuffer::Create(spec);
			}
			{
				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = {
				   { ShaderDataType::Float3, "a_Position" },
				};
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

		s_Data->DeferredLightingPass->SetInput("CBCamera", s_Data->CameraDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBScene", s_Data->SceneDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBPointLight", s_Data->PointLightDataBuffer);
		s_Data->DeferredLightingPass->SetInput("CBSpotLight", s_Data->SpotLightDataBuffer);
		s_Data->DeferredLightingPass->SetInput("u_AlbedoBuffer", s_Data->DeferredGeoPass->GetOutput(0));
		s_Data->DeferredLightingPass->SetInput("u_MREBuffer", s_Data->DeferredGeoPass->GetOutput(1));
		s_Data->DeferredLightingPass->SetInput("u_NormalBuffer", s_Data->DeferredGeoPass->GetOutput(2));
		s_Data->DeferredLightingPass->SetInput("u_PositionBuffer", s_Data->DeferredGeoPass->GetOutput(3));
		s_Data->DeferredLightingPass->SetInput("u_BRDFLut", Renderer::GetTexture<Texture2D>("BRDFLut"));

		s_Data->ShadowMapPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->ShadowMapPass->SetInput("CBShadow", s_Data->ShadowDataBuffer);

		s_Data->ShadowMappingPass->SetInput("CBShadow", s_Data->ShadowDataBuffer);
		s_Data->ShadowMappingPass->SetInput("u_NormalBuffer", s_Data->DeferredGeoPass->GetOutput(2));
		s_Data->ShadowMappingPass->SetInput("u_PositionBuffer", s_Data->DeferredGeoPass->GetOutput(3));
		s_Data->ShadowMappingPass->SetInput("u_ShadowDepth", s_Data->ShadowMapPass->GetDepthOutput());
		s_Data->ShadowMappingPass->SetInput("u_LightResult", s_Data->DeferredLightingPass->GetOutput());

		s_Data->CompositePass->SetInput("u_Color", s_Data->ShadowMappingPass->GetOutput());

		s_Data->SkyboxPass->SetInput("CBCamera", s_Data->CameraDataBuffer);

		Ref<MaterialAsset> defaultMaterialAsset = CreateRef<MaterialAsset>();
		s_Data->DefaultMaterial = defaultMaterialAsset->GetMaterial();
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
			//s_Data->DeferredGeoPass->GetTargetFramebuffer()->Resize(width, height);
			//s_Data->DeferredLightingPass->GetTargetFramebuffer()->Resize(width, height);
			//s_Data->ShadowMappingPass->GetTargetFramebuffer()->Resize(width, height);
			//s_Data->CompositePass->GetTargetFramebuffer()->Resize(width, height);
		}
	}

	void SceneRenderer::BeginScene(const Ref<Environment> environment)
	{
		s_Data->CameraData.ViewProj = environment->ViewProjection;
		s_Data->CameraData.CameraPosition = environment->CameraPosition;
		s_Data->CameraDataBuffer->SetData(&s_Data->CameraData);

		s_Data->SceneEnvironment = environment;

		// Sky Light
		{
			// TODO: Handle SceneEnvironment Map
			s_Data->SceneData.SkyLightIntensity = environment->SkyLightIntensity;
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

		if (environment->EnvMapHandle)
		{
			Ref<EnvMapAsset> asset = AssetManager::GetAsset<EnvMapAsset>(environment->EnvMapHandle);
			Ref<TextureCube> textureCube = asset->GetIrradianceMap();
			s_Data->SkyboxPass->SetInput("u_EnvMap", textureCube);
		}
		else
		{
			s_Data->SkyboxPass->SetInput("u_EnvMap", Renderer::GetTexture<TextureCube>("BlackCube"));
		}
	}

	void SceneRenderer::EndScene()
	{
		DeferredGeoPass();
		DeferredLightPass();

		ShadowMapPass();
		ShadowMappingPass();

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

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<MaterialAsset>& material)
	{
		s_Data->DrawCommands.push_back({ mesh ,material->GetMaterial() ,{
			transform,
			material->GetAlbedo(),
			material->GetEmission(),
			material->GetMetalness(),
			material->GetRoughness(),
			material->IsUsingNormalMap() } });
	}

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		s_Data->DrawCommands.push_back({ mesh ,s_Data->DefaultMaterial ,{
			transform,
			glm::vec3(1.0f),
			0.0f,
			0.0f,
			0.0f,
			false  } });
	}

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, MeshComponent& meshComponent, AssetHandle materialAssetHandle)
	{
		if (meshComponent.MeshSourceHandle)
		{
			if (!AssetManager::IsAssetHandleValid(meshComponent.MeshHandle))
			{
				Ref<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>(meshComponent.MeshSourceHandle);
				meshComponent.MeshHandle = AssetManager::CreateMemoryOnlyAsset<Mesh>(meshSource);
			}

			Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshComponent.MeshHandle);

			if (AssetManager::IsAssetHandleValid(materialAssetHandle))
			{
				Ref<MaterialAsset> materialAsset = AssetManager::GetAsset<MaterialAsset>(materialAssetHandle);
				SubmitStaticMesh(transform, mesh, materialAsset);
			}
			else
			{
				SubmitStaticMesh(transform, mesh);
			}
		}
	}

	void SceneRenderer::ExecuteDrawCommands(bool useMaterial)
	{
		for (auto& command : s_Data->DrawCommands)
		{
			s_Data->ModelData.Transform = command.ModelData.Transform;
			s_Data->ModelData.Material = command.ModelData.Material;

			s_Data->ModelDataBuffer->SetData(&s_Data->ModelData);

			if (useMaterial)
				Renderer::DrawMesh(command.Mesh, command.Material);
			else
				Renderer::DrawMesh(command.Mesh);
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

	void SceneRenderer::ShadowMapPass()
	{
		// Directional Light
		{
			Renderer::BeginRenderPass(s_Data->ShadowMapPass);
			s_Data->ShadowData.LightType = static_cast<uint32_t>(LightComponent::LightType::Directional);
			s_Data->ShadowData.ShadowType = static_cast<uint32_t>(s_Data->SceneEnvironment->DirLight.ShadowType);
			if (s_Data->SceneEnvironment->DirLight.ShadowType != LightComponent::ShadowType::None)
			{
				glm::vec3 lightPosition = glm::vec3(0.0f) - (s_Data->SceneEnvironment->DirLight.Direction * 20.0f);
				glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 lightOrthoMatrix = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
				s_Data->ShadowData.LightViewProj = lightOrthoMatrix * lightViewMatrix;

				s_Data->ShadowDataBuffer->SetData(&s_Data->ShadowData);

				ExecuteDrawCommands(false);
			}
			else
			{
				s_Data->ShadowDataBuffer->SetData(&s_Data->ShadowData);
			}
			Renderer::EndRenderPass();
		}

		// TODO: Implement Point Light Shadow and Spot Light Shadow
	}

	void SceneRenderer::ShadowMappingPass()
	{
		Renderer::BeginRenderPass(s_Data->ShadowMappingPass);
		Renderer::DrawFullScreenQuad();
		Renderer::EndRenderPass();
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
}