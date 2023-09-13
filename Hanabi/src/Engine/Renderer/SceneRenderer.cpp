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
			float padding;
		};

		struct PointLight
		{
			glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
			float Intensity = 0.0f;
			glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
			float Radius;
			float Falloff;

			// Padding
			float padding[3];
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
			float padding[2];
		};

		struct CBModel
		{
			glm::mat4 Transform;
			bool UseNormalMap = false;

			// Padding
			float padding[3];
		};

		struct CBCamera
		{
			glm::mat4 ViewProj;
			glm::vec3 CameraPosition;

			// Padding
			float padding;
		};

		struct CBScene
		{
			float SkyLightIntensity = 0.0f;

			// Padding
			float padding[3];

			DirectionalLight Light;
		};

		struct CBPointLight
		{
			uint32_t Count = 0;

			// Padding
			float padding[3];

			PointLight PointLights[MAX_POINT_LIGHT]{};
		};

		struct CBSpotLight
		{
			uint32_t Count = 0;

			// Padding
			float padding[3];

			SpotLight SpotLights[MAX_SPOT_LIGHT]{};
		};

		struct CBShadow
		{
			glm::mat4 LightViewProj;
			uint32_t ShadowType = 0;

			// Padding
			float padding[3];
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

		Ref<RenderPass> SkyboxPass;
		Ref<RenderPass> ShadowPass;
		Ref<RenderPass> GeoPass;
		Ref<RenderPass> DeferredGeoPass;

		Ref<Material> DefaultMaterial;

		struct GeoDrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<Material> Material;

			CBModel ModelData;
		};
		std::vector<GeoDrawCommand> DrawCommands;

		bool SkyboxDrawRequested = false;
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

		// Geometry Pass
		Ref<Framebuffer> geoFramebuffer;
		{
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::RGBA8F,ImageFormat::MousePick,ImageFormat::Depth };
				spec.Width = 1920;
				spec.Height = 1080;
				spec.SwapChainTarget = false;
				// TODO: Make a better way to do mouse picking
				spec.MousePickClearValue = -1;
				geoFramebuffer = Framebuffer::Create(spec);
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
				pipelineSpec.Shader = Renderer::GetDefaultShader();
				pipelineSpec.TargetFramebuffer = geoFramebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->GeoPass = RenderPass::Create(renderPassSpec);
			}
		}
		{
			Ref<Framebuffer> geoFramebuffer;
			{
				{
					FramebufferSpecification spec;
					spec.Attachments = { ImageFormat::RGBA8F,ImageFormat::RGBA8F,
						ImageFormat::RGBA8F,ImageFormat::RGBA8F,ImageFormat::Depth };
					spec.Width = 1920;
					spec.Height = 1080;
					spec.SwapChainTarget = false;
					geoFramebuffer = Framebuffer::Create(spec);
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
					pipelineSpec.TargetFramebuffer = geoFramebuffer;
					pipelineSpec.BackfaceCulling = true;
					pipelineSpec.DepthTest = true;
					pipelineSpec.Topology = PrimitiveTopology::Triangles;
					pipelineSpec.DepthOperator = DepthCompareOperator::Less;

					RenderPassSpecification renderPassSpec;
					renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
					s_Data->DeferredGeoPass = RenderPass::Create(renderPassSpec);
				}
			}
		}
	
		// Skybox Pass
		{
			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = {
			   { ShaderDataType::Float3, "a_Position" },
			};
			pipelineSpec.Shader = Renderer::GetShader("Skybox");
			pipelineSpec.TargetFramebuffer = geoFramebuffer;
			pipelineSpec.BackfaceCulling = false;
			pipelineSpec.DepthTest = true;
			pipelineSpec.Topology = PrimitiveTopology::Triangles;
			pipelineSpec.DepthOperator = DepthCompareOperator::LessEqual;

			RenderPassSpecification renderPassSpec;
			renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
			s_Data->SkyboxPass = RenderPass::Create(renderPassSpec);
		}

		// Shadow Pass	
		{
			Ref<Framebuffer> shadowFramebuffer;
			{
				FramebufferSpecification spec;
				spec.Attachments = { ImageFormat::ShadowMap };
				spec.Width = 2048;
				spec.Height = 2048;
				spec.SwapChainTarget = false;
				spec.DepthClearValue = 1.0f;
				shadowFramebuffer = Framebuffer::Create(spec);
			}
			{
				VertexBufferLayout vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
				};

				PipelineSpecification pipelineSpec;
				pipelineSpec.Layout = vertexLayout;
				pipelineSpec.Shader = Renderer::GetShader("DirShadowMap");
				pipelineSpec.TargetFramebuffer = shadowFramebuffer;
				pipelineSpec.BackfaceCulling = true;
				pipelineSpec.DepthTest = true;
				pipelineSpec.Topology = PrimitiveTopology::Triangles;
				pipelineSpec.DepthOperator = DepthCompareOperator::Less;

				RenderPassSpecification renderPassSpec;
				renderPassSpec.Pipeline = Pipeline::Create(pipelineSpec);
				s_Data->ShadowPass = RenderPass::Create(renderPassSpec);
			}
		}

		s_Data->GeoPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->GeoPass->SetInput("CBCamera", s_Data->CameraDataBuffer);
		s_Data->GeoPass->SetInput("CBScene", s_Data->SceneDataBuffer);
		s_Data->GeoPass->SetInput("CBPointLight", s_Data->PointLightDataBuffer);
		s_Data->GeoPass->SetInput("CBSpotLight", s_Data->SpotLightDataBuffer);
		s_Data->GeoPass->SetInput("u_ShadowDepth", s_Data->ShadowPass->GetDepthOutput());
		s_Data->GeoPass->SetInput("CBShadow", s_Data->ShadowDataBuffer);

		s_Data->ShadowPass->SetInput("CBModel", s_Data->ModelDataBuffer);
		s_Data->ShadowPass->SetInput("CBShadow", s_Data->ShadowDataBuffer);

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
		const FramebufferSpecification& fbsc = s_Data->GeoPass->GetTargetFramebuffer()->GetSpecification();
		if (fbsc.Width != width || fbsc.Height != height)
		{
			s_Data->GeoPass->GetTargetFramebuffer()->Resize(width, height);
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

		switch (environment->ClearType)
		{
		case CameraComponent::ClearMethod::None:
		{
			constexpr glm::vec4 clearColor = { 0.0f,0.0f,0.0f,1.0f };
			Renderer::SetClearColor(clearColor);
			s_Data->SkyboxPass->SetInput("u_SkyboxTexture", Renderer::GetTexture<TextureCube>("BlackCube"));
			s_Data->SkyboxDrawRequested = false;
			break;
		}
		case CameraComponent::ClearMethod::Soild_Color:
		{
			Renderer::SetClearColor(environment->ClearColor);

			s_Data->SkyboxDrawRequested = false;
			break;
		}
		case CameraComponent::ClearMethod::Skybox:
		{
			s_Data->SkyboxDrawRequested = true;
			if (environment->SkyboxAssetHandle)
			{
				Ref<EnvMapAsset> asset = AssetManager::GetAsset<EnvMapAsset>(environment->SkyboxAssetHandle);
				Ref<TextureCube> textureCube = asset->GetEnvMap();
				s_Data->SkyboxPass->SetInput("u_SkyboxTexture", textureCube);
			}
			else
			{
				s_Data->SkyboxPass->SetInput("u_SkyboxTexture", Renderer::GetTexture<TextureCube>("BlackCube"));
			}
			break;
		}
		}
	}

	void SceneRenderer::EndScene()
	{
		// Temp deferred rendering
		{
			Renderer::BeginRenderPass(s_Data->DeferredGeoPass);
			ExecuteDrawCommands();
			Renderer::EndRenderPass(s_Data->DeferredGeoPass);
		}

		ShadowPass();
		GeometryPass();
		SkyboxPass();
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data->GeoPass;
	}

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<MaterialAsset>& material)
	{
		s_Data->DrawCommands.push_back({ mesh ,material->GetMaterial() ,{ transform,material->IsUsingNormalMap() } });
	}

	void SceneRenderer::SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		s_Data->DrawCommands.push_back({ mesh ,s_Data->DefaultMaterial ,{ transform,false } });
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

	void SceneRenderer::ExecuteDrawCommands()
	{
		for (auto& command : s_Data->DrawCommands)
		{
			s_Data->ModelData.Transform = command.ModelData.Transform;
			s_Data->ModelData.UseNormalMap = command.ModelData.UseNormalMap;

			s_Data->ModelDataBuffer->SetData(&s_Data->ModelData);

			Renderer::DrawMesh(command.Mesh, command.Material);
		}
	}

	void SceneRenderer::ShadowPass()
	{
		// Directional Light
		{
			Renderer::BeginRenderPass(s_Data->ShadowPass);
			s_Data->ShadowData.ShadowType = static_cast<uint32_t>(s_Data->SceneEnvironment->DirLight.ShadowType);
			if (s_Data->SceneEnvironment->DirLight.ShadowType != LightComponent::ShadowType::None)
			{
				glm::vec3 lightPosition = glm::vec3(0.0f) - (s_Data->SceneEnvironment->DirLight.Direction * 20.0f);
				glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 lightOrthoMatrix = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
				s_Data->ShadowData.LightViewProj = lightOrthoMatrix * lightViewMatrix;

				s_Data->ShadowDataBuffer->SetData(&s_Data->ShadowData);

				ExecuteDrawCommands();
			}
			else
			{
				s_Data->ShadowDataBuffer->SetData(&s_Data->ShadowData);
			}
			Renderer::EndRenderPass(s_Data->ShadowPass);
		}

		// TODO: Implement Point Light Shadow and Spot Light Shadow
	}

	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(s_Data->GeoPass);
		ExecuteDrawCommands();
		Renderer::EndRenderPass(s_Data->GeoPass);
	}

	void SceneRenderer::SkyboxPass()
	{
		if (s_Data->SkyboxDrawRequested)
		{
			Renderer::BeginRenderPass(s_Data->SkyboxPass, false);

			Ref<Mesh> mesh = Renderer::GetMesh("Box");
			Renderer::DrawMesh(mesh, s_Data->DefaultMaterial);

			Renderer::EndRenderPass(s_Data->SkyboxPass);
		}
	}

	void SceneRenderer::CompositePass()
	{}
}