#include "hnbpch.h"
#include "Application.h"

namespace Hanabi
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		HNB_CORE_TRACE("{0}", e);
	}

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
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}

