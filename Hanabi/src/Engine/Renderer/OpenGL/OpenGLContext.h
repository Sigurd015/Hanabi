#pragma once
#include "Engine/Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace Hanabi
{
	class OpenGLContext :public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		void Init() override;
		void SwapBuffer() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}