#include "hnbpch.h"
#include "Engine/Platform/Window.h"

#ifdef HNB_PLATFORM_WINDOWS
#include "Engine/Platform/Windows/WindowsWnd.h"
#endif

namespace Hanabi
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef HNB_PLATFORM_WINDOWS
		return CreateScope<WindowsWnd>(props);
#else
		HNB_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}