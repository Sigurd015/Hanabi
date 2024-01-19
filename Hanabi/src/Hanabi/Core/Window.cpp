#include "hnbpch.h"
#include "Window.h"
#include "Hanabi/Events/ApplicationEvent.h"
#include "Hanabi/Events/KeyEvent.h"
#include "Hanabi/Events/MouseEvent.h"
#include "Hanabi/Renderer/RendererContext.h"
#include "Hanabi/Renderer/RendererAPI.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include <GLFW/glfw3native.h>
#endif

namespace Hanabi
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<Window>(props);
	}

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		HNB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const WindowProps& props) :m_Data(props)
	{
		Init();
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffer(m_Data.VSync);
	}

	void Window::SetWindowTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, (title + " - " + m_Data.Title).c_str());
	}

	void Window::SetVSync(bool enable)
	{
		m_Data.VSync = enable;
	}

	bool Window::IsVSync() const
	{
		return m_Data.VSync;
	}

	void Window::Init()
	{
		HNB_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			HNB_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		switch (RendererAPI::GetAPI())
		{
#ifdef HNB_ENABLE_VULKAN
		case RendererAPIType::Vulkan:
			m_Data.Title += "<Vulkan>";
			break;
#endif
#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			m_Data.Title += "<DX11>";
			break;
#endif
		}

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;

		switch (RendererAPI::GetAPI())
		{
#ifdef HNB_ENABLE_VULKAN
		case RendererAPIType::Vulkan:
			m_Context = RendererContext::Create(m_Window, m_Data.Width, m_Data.Height, m_Data.VSync);
			break;
#endif
#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			HWND winWnd = glfwGetWin32Window(m_Window);
			m_Context = RendererContext::Create(&winWnd, m_Data.Width, m_Data.Height, m_Data.VSync);
			break;
#endif
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event((KeyCode)key, false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event((KeyCode)key, true);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event((KeyCode)keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event((MouseButton)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event((MouseButton)button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int pathCount, const char* paths[])
			{
				WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

				std::vector<std::filesystem::path> filepaths(pathCount);
				for (int i = 0; i < pathCount; i++)
					filepaths[i] = paths[i];

				WindowDropEvent event(std::move(filepaths));
				data.EventCallback(event);
			});
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}
}