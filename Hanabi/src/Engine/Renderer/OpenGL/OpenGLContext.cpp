#include "hnbpch.h"
#include "Engine/Renderer/OpenGL/OpenGLContext.h"

#include <glad/glad.h>

namespace Hanabi
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) :m_WindowHandle(windowHandle)
	{
		HNB_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		HNB_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HNB_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffer()
	{
		HNB_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}