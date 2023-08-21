#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "EnvMapAsset.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	enum CBBingSlot : uint32_t
	{
		MODEL = 0,
		CAMERA = 1,
		SCENE = 2,
		POINT_LIGHT = 3,
		SPOT_LIGHT = 4
	};

	struct SceneRendererData
	{
		struct CBCamera
		{
			glm::mat4 ViewProj;
			glm::vec3 CameraPosition;

			// Padding
			float padding;
		};

		struct CBScene
		{
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

		CBCamera CameraData;
		CBScene SceneData;
		CBPointLight PointLightData;
		CBSpotLight SpotLightData;

		Ref<ConstantBuffer> ModelDataBuffer;
		Ref<ConstantBuffer> CameraDataBuffer;
		Ref<ConstantBuffer> SceneDataBuffer;
		Ref<ConstantBuffer> PointLightDataBuffer;
		Ref<ConstantBuffer> SpotLightDataBuffer;

		Ref<RenderPass> SkyboxPass;
		Ref<RenderPass> ShadowPass;
		Ref<RenderPass> GeoPass;

		Ref<Material> DefaultMaterial;
		Ref<Material> SkyboxMaterial;

		Ref<Pipeline> ShadowPipeline;
		Ref<Pipeline> GeoPipeline;
		Ref<Pipeline> SkyboxPipeline;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<Material> Material;

			CBModel ModelData;
		};
		std::vector<DrawCommand> DrawCommands;

		bool SkyboxDrawRequested = false;
	};
	static SceneRendererData* s_Data;

	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData();

		s_Data->ModelDataBuffer = ConstantBuffer::Create(sizeof(CBModel), CBBingSlot::MODEL);
		s_Data->CameraDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBCamera), CBBingSlot::CAMERA);
		s_Data->SceneDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBScene), CBBingSlot::SCENE);
		s_Data->PointLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBPointLight), CBBingSlot::POINT_LIGHT);
		s_Data->SpotLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBSpotLight), CBBingSlot::SPOT_LIGHT);

		Ref<Framebuffer> geoFramebuffer;
		{
			FramebufferSpecification spec;
			spec.Attachments = { ImageFormat::RGBA8F,ImageFormat::MousePick,ImageFormat::Depth };
			spec.Width = 1920;
			spec.Height = 1080;
			// TODO: Make a better way to do mouse picking
			spec.MousePickClearValue = -1;
			geoFramebuffer = Framebuffer::Create(spec);
		}

		Ref<Framebuffer> shadowFramebuffer;
		{
			FramebufferSpecification spec;
			spec.Attachments = { ImageFormat::ShadowMap };
			spec.Width = 1920;
			spec.Height = 1080;
			shadowFramebuffer = Framebuffer::Create(spec);
		}

		//TODO: Make layout dynamic	
		// Geometry Pipeline
		{
			PipelineSpecification spec;
			VertexBufferLayout layout = {
			   { ShaderDataType::Float3, "a_Position" },
			   { ShaderDataType::Float3, "a_Normal" },
			   { ShaderDataType::Float3, "a_Tangent" },
			   { ShaderDataType::Float3, "a_Bitangent" },
			   { ShaderDataType::Float2, "a_TexCoord" },
			};

			spec.Layout = layout;
			spec.Shader = Renderer::GetDefaultShader();
			spec.TargetFramebuffer = geoFramebuffer;
			spec.BackfaceCulling = true;
			spec.DepthTest = true;
			spec.Topology = PrimitiveTopology::Triangles;
			spec.DepthOperator = DepthCompareOperator::Less;
			s_Data->GeoPipeline = Pipeline::Create(spec);
		}

		// Skybox Pipeline
		{
			PipelineSpecification spec;
			VertexBufferLayout layout = {
			   { ShaderDataType::Float3, "a_Position" },
			};
			spec.Layout = layout;
			spec.Shader = Renderer::GetShader("Skybox");
			spec.TargetFramebuffer = geoFramebuffer;
			spec.BackfaceCulling = false;
			spec.DepthTest = true;
			spec.Topology = PrimitiveTopology::Triangles;
			spec.DepthOperator = DepthCompareOperator::LessEqual;
			s_Data->SkyboxPipeline = Pipeline::Create(spec);
		}

		// Dont change the order of these, specially the model data buffer
		s_Data->GeoPipeline->SetConstantBuffer(s_Data->ModelDataBuffer);
		s_Data->GeoPipeline->SetConstantBuffer(s_Data->CameraDataBuffer);
		s_Data->GeoPipeline->SetConstantBuffer(s_Data->SceneDataBuffer);
		s_Data->GeoPipeline->SetConstantBuffer(s_Data->PointLightDataBuffer);
		s_Data->GeoPipeline->SetConstantBuffer(s_Data->SpotLightDataBuffer);

		s_Data->SkyboxPipeline->SetConstantBuffer(s_Data->CameraDataBuffer);

		// Skybox Pass
		{
			RenderPassSpecification spec;
			spec.Pipeline = s_Data->SkyboxPipeline;
			s_Data->SkyboxPass = RenderPass::Create(spec);
		}

		// Geometry Pass
		{
			RenderPassSpecification spec;
			spec.Pipeline = s_Data->GeoPipeline;
			s_Data->GeoPass = RenderPass::Create(spec);
		}

		// Shadow Pass
		{
			RenderPassSpecification spec;
			spec.Pipeline = s_Data->ShadowPipeline;
			s_Data->ShadowPass = RenderPass::Create(spec);
		}

		Ref<MaterialAsset> defaultMaterialAsset = CreateRef<MaterialAsset>();
		s_Data->DefaultMaterial = defaultMaterialAsset->GetMaterial();
		s_Data->SkyboxMaterial = CreateRef<Material>(Renderer::GetShader("Skybox"));
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

	void SceneRenderer::BeginScene(const Environment& environment)
	{
		s_Data->CameraData.ViewProj = environment.ViewProjection;
		s_Data->CameraData.CameraPosition = environment.CameraPosition;
		s_Data->CameraDataBuffer->SetData(&s_Data->CameraData);

		s_Data->SceneData.Light = environment.DirLight;
		s_Data->SceneDataBuffer->SetData(&s_Data->SceneData);

		s_Data->PointLightData.Count = environment.PointLights.size();
		for (uint32_t i = 0; i < s_Data->PointLightData.Count; i++)
		{
			s_Data->PointLightData.PointLights[i] = environment.PointLights[i];
		}
		s_Data->PointLightDataBuffer->SetData(&s_Data->PointLightData);

		s_Data->SpotLightData.Count = environment.SpotLights.size();
		for (uint32_t i = 0; i < s_Data->SpotLightData.Count; i++)
		{
			s_Data->SpotLightData.SpotLights[i] = environment.SpotLights[i];
		}
		s_Data->SpotLightDataBuffer->SetData(&s_Data->SpotLightData);
		s_Data->DrawCommands.clear();

		switch (environment.ClearType)
		{
		case CameraComponent::ClearMethod::Soild_Color:
		{
			Renderer::SetClearColor(environment.ClearColor);

			s_Data->SkyboxDrawRequested = false;
			break;
		}
		case CameraComponent::ClearMethod::Skybox:
		{
			s_Data->SkyboxDrawRequested = true;
			if (environment.SkyboxAssetHandle)
			{
				Ref<EnvMapAsset> asset = AssetManager::GetAsset<EnvMapAsset>(environment.SkyboxAssetHandle);
				Ref<TextureCube> textureCube = asset->GetEnvMap();
				s_Data->SkyboxMaterial->SetTexture<TextureCube>(textureCube);
			}
			else
			{
				s_Data->SkyboxMaterial->SetTexture<TextureCube>(Renderer::GetTexture<TextureCube>("BlackCube"));
			}
			break;
		}
		}
	}

	void SceneRenderer::EndScene()
	{
		//Renderer::BeginRenderPass(s_Data->ShadowPass);
		//for (auto& command : s_Data->DrawCommands)
		//{
		//}
		//Renderer::EndRenderPass(s_Data->ShadowPass);

		Renderer::BeginRenderPass(s_Data->GeoPass);
		for (auto& command : s_Data->DrawCommands)
		{
			// TODO: Why only heap allocation can get the correct result?
			Ref<CBModel> modelData = CreateRef<CBModel>();
			modelData->Transform = command.ModelData.Transform;
			modelData->UseNormalMap = command.ModelData.UseNormalMap;

			Ref<ConstantBuffer> transformBuffer = s_Data->GeoPipeline->GetConstantBuffer(CBBingSlot::MODEL);
			transformBuffer->SetData(modelData.get());

			Renderer::SubmitStaticMesh(command.Mesh, command.Material, s_Data->GeoPipeline);
		}
		Renderer::EndRenderPass(s_Data->GeoPass);
		if (s_Data->SkyboxDrawRequested)
		{
			Renderer::BeginRenderPass(s_Data->SkyboxPass, false);

			Ref<Mesh> mesh = Renderer::GetMesh("Box");
			Renderer::SubmitStaticMesh(mesh, s_Data->SkyboxMaterial, s_Data->SkyboxPipeline);

			Renderer::EndRenderPass(s_Data->SkyboxPass);
		}
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

			if (materialAssetHandle)
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
}