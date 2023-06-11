#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"

namespace Hanabi
{
	static RenderCommandQueue* s_CommandQueue = nullptr;
	static Scope<RendererAPI> s_RendererAPI = nullptr;

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		s_CommandQueue = new RenderCommandQueue();
		s_RendererAPI = RendererAPI::Create();
		s_RendererAPI->Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		delete s_CommandQueue;
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		s_RendererAPI->SetClearColor(color);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass> renderPass)
	{
		Renderer::Submit([renderPass]()
			{
				s_RendererAPI->BeginRenderPass(renderPass);
			});
	}

	void Renderer::EndRenderPass(const Ref<RenderPass> renderPass)
	{
		Renderer::Submit([renderPass]()
			{
				s_RendererAPI->EndRenderPass(renderPass);
			});
	}

	void Renderer::BeginRender()
	{
		Renderer::Submit([]()
			{
				s_RendererAPI->BeginRender();
			});
	}

	void Renderer::EndRender()
	{
		Renderer::Submit([]()
			{
				s_RendererAPI->EndRender();
			});
	}

	void Renderer::SubmitStaticMesh(const Ref<Mesh> mesh, const Ref<Pipeline> pipeline)
	{
		Renderer::Submit([mesh, pipeline]()
			{
				s_RendererAPI->SubmitStaticMesh(mesh, pipeline);
			});
	}

	void  Renderer::DrawIndexed(const Ref<VertexBuffer> vertexBuffer, const Ref<IndexBuffer> indexBuffer,
		const Ref<Pipeline> pipeline, uint32_t indexCount)
	{
		Renderer::Submit([vertexBuffer, indexBuffer, pipeline, indexCount]()
			{
				s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, pipeline, indexCount);
			});
	}

	void  Renderer::DrawLines(const Ref<VertexBuffer> vertexBuffer, const Ref<Pipeline> pipeline, uint32_t vertexCount)
	{
		Renderer::Submit([vertexBuffer, pipeline, vertexCount]()
			{
				s_RendererAPI->DrawLines(vertexBuffer, pipeline, vertexCount);
			});
	}

	void Renderer::SetLineWidth(float width)
	{
		Renderer::Submit([width]()
			{
				s_RendererAPI->SetLineWidth(width);
			});
	}

	void Renderer::WaitAndRender()
	{
		s_CommandQueue->Execute();
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return *s_CommandQueue;
	}
}