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
		m_DefaultMaterial = Material::Create(Renderer::GetShader("3DStaticMesh_Default"));
	}

	void SceneRenderer::Shutdown()
	{
		m_Renderer2D->Shutdown();
	}

	void SceneRenderer::BeginScene(const glm::mat4& viewProjection)
	{
		SceneBuffer.ViewProjection = viewProjection;

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

	void SceneRenderer::SubmitStaticMesh(const Ref<StaticMesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID)
	{
		SceneBuffer.Model = glm::transpose(transform);
		SceneConstantBuffer->SetData(&SceneBuffer, sizeof(SceneData));

		PipelineSpecification spec;
		spec.Layout = staticMesh->GetVertexBuffer()->GetLayout();
		spec.Shader = Renderer::GetShader("3DStaticMesh_Default");
		spec.Topology = PrimitiveTopology::Triangles;
		spec.RenderPass = nullptr;

		//For OpenGL
		spec.VertexBuffer = staticMesh->GetVertexBuffer();
		spec.IndexBuffer = staticMesh->GetIndexBuffer();
		//-----------------

		Ref<Pipeline> pipeline = Pipeline::Create(spec);

		pipeline->SetConstantBuffer(SceneConstantBuffer);
		Renderer::SubmitStaticMesh(staticMesh, material ? material : m_DefaultMaterial, pipeline);
	}
}