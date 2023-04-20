#include "hnbpch.h"
#include "Engine/Renderer/OpenGL/OpenGLContext.h"

#include <glad/glad.h>

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

#ifdef HNB_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		HNB_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hanabi requires at least OpenGL version 4.5!");
#endif
	}

	void OpenGLContext::SwapBuffer()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}