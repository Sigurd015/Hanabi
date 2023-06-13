#pragma once
#include "Engine/Renderer/RendererContext.h"

#include <GLFW/glfw3.h>

namespace Hanabi
{
	class OpenGLContext :public RendererContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		void SwapBuffer(bool VSync) override;
	private:
		void Init();
		GLFWwindow* m_WindowHandle;
	};
}