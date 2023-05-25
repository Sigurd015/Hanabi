#include "hnbpch.h"
#include "Engine/Layer/ImGui/ImGuiLayer.h"
#include "Engine/Renderer/RendererAPI.h"

namespace Hanabi
{
	ImGuiLayer* ImGuiLayer::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new ImGuiLayerOpenGL();

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPI::API::DX11:
			return new ImGuiLayerDX11();
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}