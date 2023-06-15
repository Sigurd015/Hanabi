#pragma once
#include "Engine/Renderer/RendererAPI.h"

namespace Hanabi
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void SetClearColor(const glm::vec4& color) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void ResetToSwapChain() override;

		void BeginRenderPass(const Ref<RenderPass>& renderPass) override;
		void EndRenderPass(const Ref<RenderPass>& renderPass) override;
		void SubmitStaticMesh(const Ref<StaticMesh>& mesh, const Ref<Pipeline>& pipeline) override;
		void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			const Ref<Pipeline>& pipeline, uint32_t indexCount = 0) override;
		void DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount) override;
	private:
		void Clear();
		glm::vec4 m_ClearColor = { 0.3f,0.3f,0.3f,1.0f };
		Ref<RenderPass> m_RenderPass;
	};
}