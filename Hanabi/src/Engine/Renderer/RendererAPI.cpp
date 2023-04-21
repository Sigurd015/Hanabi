#include "hnbpch.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/OpenGL/OpenGLAPI.h"

namespace Hanabi
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}