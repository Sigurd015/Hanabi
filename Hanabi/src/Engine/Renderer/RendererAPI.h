#pragma once
#include "Engine/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Hanabi
{
	enum class RendererAPIType
	{
		None = 0, OpenGL = 1, Vulkan = 2, DX11 = 3, DX12 = 4
	};

	struct	RendererConfig
	{
		RendererAPIType APIType;
	};

	class RendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void SetLineWidth(float width) = 0;
		static RendererAPIType GetAPI() { return s_API; }
		static void SetAPI(RendererAPIType api) { s_API = api; };
		static Scope<RendererAPI> Create();
	private:
		static RendererAPIType s_API;
	};
}