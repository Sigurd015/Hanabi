#include "hnbpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Hanabi
{
	Application::Application()
	{}

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
		while (true)
		{

		}
	}
}

