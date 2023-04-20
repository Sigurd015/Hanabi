#include "hnbpch.h"
#include "Engine/Input/Input.h"

#ifdef HNB_PLATFORM_WINDOWS
#include "Engine/Platform/Windows/WindowsInput.h"
#endif

namespace Hanabi
{

	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
#ifdef HNB_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
#else
		HNB_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}