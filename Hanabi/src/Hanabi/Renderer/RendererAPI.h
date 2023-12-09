#pragma once
#include "Mesh.h"
#include "Hanabi/Renderer/RenderPass.h"

#include <glm/glm.hpp>

namespace Hanabi
{
	enum class RendererAPIType
	{
		None = 0, Vulkan = 1, DX12 = 2, Metal = 3, DX11 = 4,
	};

	class RendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t width, uint32_t height) = 0;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear) = 0;
		virtual void EndRenderPass() = 0;

		virtual void DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material) = 0;
		virtual void DrawMesh(const Ref<Mesh>& mesh) = 0;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			uint32_t indexCount) = 0;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount) = 0;
		virtual void DrawFullScreenQuad() = 0;

		virtual std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap) = 0;

		static RendererAPIType GetAPI() { return s_API; }
		static void SetAPI(RendererAPIType api) { s_API = api; };
		static Scope<RendererAPI> Create();
	private:
		static RendererAPIType s_API;
	};
}