#include "hnbpch.h"
#include "Win32.h"

namespace Hanabi
{
	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWnd(props);
	}

	WindowsWnd::WindowsWnd(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWnd::~WindowsWnd()
	{
		Shutdown();
	}

	void WindowsWnd::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWnd::SetVSync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enable;
	}

	bool WindowsWnd::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWnd::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		HNB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			HNB_CORE_ASSERT(success, "Could not intialize GLFW!");
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void WindowsWnd::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}