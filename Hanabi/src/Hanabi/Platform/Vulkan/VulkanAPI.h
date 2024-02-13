#pragma once
#include "Hanabi/Renderer/RendererAPI.h"

namespace Hanabi
{
	class VulkanAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t width, uint32_t height) override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear) override;
		virtual void EndRenderPass() override;
		virtual void BeginComputePass(const Ref<ComputePass>& computePass) override;
		virtual void EndComputePass(const Ref<ComputePass>& computePass) override;

		virtual void DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material) override;
		virtual void DrawMesh(const Ref<Mesh>& mesh) override;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			uint32_t indexCount = 0) override;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount) override;
		virtual void DrawFullScreenQuad() override;

		virtual std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap) override;
	};
}