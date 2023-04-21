#include "hnbpch.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/OpenGL/OpenGLAPI.h"

namespace Hanabi
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}