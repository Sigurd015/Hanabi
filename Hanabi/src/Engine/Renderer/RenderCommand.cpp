#include "hnbpch.h"
#include "RenderCommand.h"
#include "OpenGL/OpenGLAPI.h"

namespace Hanabi
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}