#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"

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

		struct CBModel
		{
			glm::mat4 Transform;
		};

		struct CBScene
		{
			DirectionalLight Light;
		};

		struct CBPointLight
		{
			PointLight PointLights[MAX_POINT_LIGHT]{};
			uint32_t Count = 0;

			// Padding
			float padding[3];
		};

		struct CBSpotLight
		{
			SpotLight SpotLights[MAX_SPOT_LIGHT]{};
			uint32_t Count = 0;

			// Padding
			float padding[3];
		};

		CBCamera CameraData;
		CBModel ModelData;
		CBScene SceneData;
		CBPointLight PointLightData;
		CBSpotLight SpotLightData;

		Hanabi::Ref<Hanabi::ConstantBuffer> ModelDataBuffer;
		Hanabi::Ref<Hanabi::ConstantBuffer> CameraDataBuffer;
		Hanabi::Ref<Hanabi::ConstantBuffer> SceneDataBuffer;
		Hanabi::Ref<Hanabi::ConstantBuffer> PointLightDataBuffer;
		Hanabi::Ref<Hanabi::ConstantBuffer> SpotLightDataBuffer;

		Ref<RenderPass> GeoPass;

		Ref<Material> m_DefaultMaterial;
		Ref<Pipeline> m_DefaultPipeline;
	};
	static SceneRendererData* s_Data;

	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData();

		s_Data->ModelDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBModel), CBBingSlot::MODEL);
		s_Data->CameraDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBCamera), CBBingSlot::CAMERA);
		s_Data->SceneDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBScene), CBBingSlot::SCENE);
		s_Data->PointLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBPointLight), CBBingSlot::POINT_LIGHT);
		s_Data->SpotLightDataBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::CBSpotLight), CBBingSlot::SPOT_LIGHT);

		FramebufferSpecification geoFramebufferSpec;
		geoFramebufferSpec.Attachments = {
			FramebufferTextureFormat::RGBA8F,
			FramebufferTextureFormat::MousePick,
			FramebufferTextureFormat::Depth };
		geoFramebufferSpec.Width = 1920;
		geoFramebufferSpec.Height = 1080;
		// TODO: Make a better way to do mouse picking
		// TODO: Make a better way to clear the framebuffer and use the different clear values for editor and game
		geoFramebufferSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		geoFramebufferSpec.MousePickClearValue = -1;

		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		s_Data->GeoPass = RenderPass::Create(geoRenderPassSpec);

		s_Data->m_DefaultMaterial = Renderer::GetDefaultMaterial();

		//TODO: Make layout dynamic
		VertexBufferLayout layout = {
		  { ShaderDataType::Float3, "a_Position" },
		  { ShaderDataType::Float3, "a_Normal" },
		  { ShaderDataType::Float2, "a_TexCoord" },
		};
		PipelineSpecification spec;
		spec.Layout = layout;
		spec.Shader = Renderer::GetShader("PhongLighting");
		spec.Topology = PrimitiveTopology::Triangles;
		s_Data->m_DefaultPipeline = Pipeline::Create(spec);

		// Dont change the order of the constant buffers, this order is mathcing the binding order in the shader
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->ModelDataBuffer);
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->CameraDataBuffer);
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->SceneDataBuffer);
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->PointLightDataBuffer);
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->SpotLightDataBuffer);
	}

	void SceneRenderer::Shutdown()
	{
		delete s_Data;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		const FramebufferSpecification& fbsc = s_Data->GeoPass->GetSpecification().TargetFramebuffer->GetSpecification();
		if (fbsc.Width != width || fbsc.Height != height)
		{
			s_Data->GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
		}
	}

	void SceneRenderer::BeginScene(const Environment& environment)
	{
		s_Data->CameraData.ViewProj = environment.ViewProjection;
		s_Data->CameraData.CameraPosition = environment.CameraPosition;
		s_Data->CameraDataBuffer->SetData(&s_Data->CameraData);

		s_Data->SceneData.Light = environment.DirLight;
		s_Data->SceneDataBuffer->SetData(&s_Data->SceneData);

		s_Data->PointLightData.Count = environment.PointLightCount;
		for (uint32_t i = 0; i < environment.PointLightCount; i++)
		{
			s_Data->PointLightData.PointLights[i] = environment.PointLights[i];
		}
		s_Data->PointLightDataBuffer->SetData(&s_Data->PointLightData);

		s_Data->SpotLightData.Count = environment.SpotLightCount;
		for (uint32_t i = 0; i < environment.SpotLightCount; i++)
		{
			s_Data->SpotLightData.SpotLights[i] = environment.SpotLights[i];
		}
		s_Data->SpotLightDataBuffer->SetData(&s_Data->SpotLightData);

		Renderer::BeginRenderPass(s_Data->GeoPass);
	}

	void SceneRenderer::EndScene()
	{
		Renderer::EndRenderPass(s_Data->GeoPass);
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data->GeoPass;
	}

	void SceneRenderer::SubmitStaticMesh(const Ref<Mesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID)
	{
		Renderer::SubmitStaticMesh(staticMesh, material ? material : s_Data->m_DefaultMaterial, s_Data->m_DefaultPipeline, transform, CBBingSlot::MODEL);
	}
}