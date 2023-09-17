#pragma once
#include "Engine/Renderer/RendererAPI.h"

namespace Hanabi
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear) override;
		virtual void EndRenderPass() override;

		virtual void DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material) override;
		virtual void DrawMesh(const Ref<Mesh>& mesh) override;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			uint32_t indexCount = 0) override;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount) override;

	private:
		void Clear();
		glm::vec4 m_ClearColor = { 0.0f,0.0f,0.0f,1.0f };
		Ref<RenderPass> m_RenderPass;
	};
}