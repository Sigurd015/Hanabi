#pragma once
#include "Engine/Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace Hanabi
{
	class OpenGLContext :public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		void SwapBuffer(bool VSync) override;
	private:
		void Init();
		GLFWwindow* m_WindowHandle;
	};
}