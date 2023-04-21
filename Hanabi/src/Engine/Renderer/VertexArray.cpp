#include "hnbpch.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/OpenGL/OpenGLVertexArray.h"

namespace Hanabi
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}