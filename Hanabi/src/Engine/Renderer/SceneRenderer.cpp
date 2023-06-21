#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"

namespace Hanabi
{
	struct SceneRendererData
	{
		struct SceneData
		{
			glm::mat4 ViewProjection;

			// DirectionalLight
			glm::vec4 AmbientColor;
			glm::vec3 Direction;
			float Padding;
		};
		SceneData SceneBuffer;
		Ref<ConstantBuffer> SceneConstantBuffer;
		Ref<ConstantBuffer> ModelTransformConstantBuffer;

		Ref<RenderPass> GeoPass;

		Ref<Material> m_DefaultMaterial;
		Ref<Pipeline> m_DefaultPipeline;
	};
	static SceneRendererData* s_Data;


	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData();
		s_Data->SceneConstantBuffer = ConstantBuffer::Create(sizeof(SceneRendererData::SceneData), 0);
		s_Data->ModelTransformConstantBuffer = ConstantBuffer::Create(sizeof(glm::mat4), 1); // 1 is the model transform slot by default

		FramebufferSpecification geoFramebufferSpec;
		geoFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8F };
		geoFramebufferSpec.Width = 1920;
		geoFramebufferSpec.Height = 1080;
		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		s_Data->GeoPass = RenderPass::Create(geoRenderPassSpec);

		//TODO: Temp
		s_Data->m_DefaultMaterial = Material::Create(Renderer::GetShader("PhongLighting"));

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

		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->SceneConstantBuffer);
		s_Data->m_DefaultPipeline->SetConstantBuffer(s_Data->ModelTransformConstantBuffer);
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

	void SceneRenderer::BeginScene(const glm::mat4& viewProjection)
	{
		s_Data->SceneBuffer.ViewProjection = viewProjection;

		//TODO: Temp
		s_Data->SceneBuffer.AmbientColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		s_Data->SceneBuffer.Direction = glm::vec3(0.0f, 0.0f, 0.0f);

		s_Data->SceneConstantBuffer->SetData(&s_Data->SceneBuffer);
	}

	void SceneRenderer::EndScene()
	{

	}

	void SceneRenderer::SubmitStaticMesh(const Ref<Mesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID)
	{
		Renderer::SubmitStaticMesh(staticMesh, material ? material : s_Data->m_DefaultMaterial, s_Data->m_DefaultPipeline, transform);
	}
}