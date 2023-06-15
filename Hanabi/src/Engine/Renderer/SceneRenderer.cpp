#include "hnbpch.h"
#include "SceneRenderer.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"

namespace Hanabi
{
	SceneRenderer::SceneRenderer()
	{
		Init();
	}

	SceneRenderer::~SceneRenderer()
	{}

	void SceneRenderer::Init()
	{
		m_Renderer2D = CreateRef<Renderer2D>();
		m_Renderer2D->Init();
		SceneConstantBuffer = ConstantBuffer::Create(sizeof(SceneData), 0);

		//TODO: Temp
		VertexBufferLayout layout = {
		  { ShaderDataType::Float3, "a_Position" },
		  { ShaderDataType::Float3, "a_Normal" },
		  { ShaderDataType::Float2, "a_TexCoord" },
		  { ShaderDataType::Int,    "a_EntityID" }
		};
		m_DefaultShader = Shader::Create("3DStaticMesh_Default");
		m_DefaultMaterial = Material::Create(m_DefaultShader);

		PipelineSpecification spec;
		spec.Layout = layout;
		spec.Shader = m_DefaultShader;
		spec.Topology = PrimitiveTopology::Triangles;
		spec.RenderPass = nullptr;
		m_DefaultPipeline = Pipeline::Create(spec);
	}

	void SceneRenderer::Shutdown()
	{
		m_Renderer2D->Shutdown();
	}

	void SceneRenderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		SceneBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);

		switch (RendererAPI::GetAPI())
		{
#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			SceneBuffer.ViewProjection = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -1.0f)) * SceneBuffer.ViewProjection;
			SceneBuffer.ViewProjection = glm::transpose(SceneBuffer.ViewProjection);
			break;
#endif
		}

		m_Renderer2D->SetViewProjection(SceneBuffer.ViewProjection);
	}

	void SceneRenderer::BeginScene(const EditorCamera& camera)
	{
		SceneBuffer.ViewProjection = camera.GetViewProjection();

		switch (RendererAPI::GetAPI())
		{
#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			SceneBuffer.ViewProjection = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -1.0f)) * SceneBuffer.ViewProjection;
			SceneBuffer.ViewProjection = glm::transpose(SceneBuffer.ViewProjection);
			break;
#endif
		}

		m_Renderer2D->SetViewProjection(SceneBuffer.ViewProjection);
	}

	void SceneRenderer::EndScene()
	{
		m_Renderer2D->Flush();
	}

	void SceneRenderer::SubmitStaticMesh(const Ref<StaticMesh>& staticMesh, const glm::mat4& transform, int entityID)
	{
		SceneBuffer.Model = glm::transpose(transform);
		SceneConstantBuffer->SetData(&SceneBuffer, sizeof(SceneData));
		staticMesh->GetVertexBuffer()->SetLayout(m_DefaultPipeline->GetSpecification().Layout);

		//For OpenGL
		m_DefaultPipeline->GetSpecification().VertexBuffer = staticMesh->GetVertexBuffer();
		m_DefaultPipeline->GetSpecification().IndexBuffer = staticMesh->GetIndexBuffer();
		//-----------------

		m_DefaultPipeline->SetConstantBuffer(SceneConstantBuffer);
		Renderer::SubmitStaticMesh(staticMesh, m_DefaultPipeline);
	}
}