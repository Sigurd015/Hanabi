#pragma once
#include "Core.h"

namespace Hanabi
{
	class HNB_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	private:

	};

	Application* CreateApplication();
}