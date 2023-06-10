#include "hnbpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

namespace Hanabi
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) :m_WindowHandle(windowHandle)
	{
		HNB_CORE_ASSERT(windowHandle, "Window handle is null!");
		Init();
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HNB_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffer(bool VSync)
	{
		glfwSwapInterval(VSync);
		glfwSwapBuffers(m_WindowHandle);
	}
}