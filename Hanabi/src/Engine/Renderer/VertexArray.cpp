#include "hnbpch.h"
#include "VertexArray.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/OpenGL/OpenGLVertexArray.h"

namespace Hanabi
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:    HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::OpenGL:  return new OpenGLVertexArray();
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}