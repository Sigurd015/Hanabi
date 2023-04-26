#pragma once

#include <memory>
#include <filesystem>

#ifdef HNB_DEBUG
#if defined(HNB_PLATFORM_WINDOWS)
#define HNB_DEBUGBREAK() __debugbreak()
#endif
#define HNB_ENABLE_ASSERTS
#else
#define HNB_DEBUGBREAK()
#endif

#define HNB_EXPAND_MACRO(x) x
#define HNB_STRINGIFY_MACRO(x) #x

#ifdef HNB_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define HNB_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { HNB##type##ERROR(msg, __VA_ARGS__); HNB_DEBUGBREAK(); } }
#define HNB_INTERNAL_ASSERT_WITH_MSG(type, check, ...) HNB_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define HNB_INTERNAL_ASSERT_NO_MSG(type, check) HNB_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", HNB_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define HNB_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define HNB_INTERNAL_ASSERT_GET_MACRO(...) HNB_EXPAND_MACRO( HNB_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, HNB_INTERNAL_ASSERT_WITH_MSG, HNB_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define HNB_ASSERT(...) HNB_EXPAND_MACRO( HNB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define HNB_CORE_ASSERT(...) HNB_EXPAND_MACRO( HNB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define HNB_ASSERT(...)
#define HNB_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)

#define HNB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hanabi
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Engine/Core/Log.h"