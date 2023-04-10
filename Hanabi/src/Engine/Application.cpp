#include "hnbpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "GLFW/glfw3.h"
namespace Hanabi
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{}

	void Application::Run()
	{
		WindowResizeEvent e(800, 600);
		if (e.IsInCategory(EventCategoryApplication))
		{
			HNB_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			HNB_TRACE(e);
		}
		while (m_Running)
		{
			glClearColor(0, 0, 255, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
}

