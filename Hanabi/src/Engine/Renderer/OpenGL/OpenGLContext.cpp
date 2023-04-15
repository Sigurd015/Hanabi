#include "hnbpch.h"

#include <glad/glad.h>
#include "OpenGLContext.h"

namespace Hanabi
{
	Hanabi::OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) :m_WindowHandle(windowHandle)
	{
		HNB_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HNB_CORE_ASSERT(status, "Failed to initialize Glad!");

		//HNB_CORE_INFO("OpenGL Info:");
		//HNB_CORE_INFO("--Vendor: {0}", glGetString(GL_VENDOR));
		//HNB_CORE_INFO("--Renderer: {0}", glGetString(GL_RENDERER));
		//HNB_CORE_INFO("--Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffer()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}