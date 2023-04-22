#pragma once
#include <memory>

#ifdef HNB_DEBUG
#define HNB_ASSERT(x, ...) { if(!(x)) { HNB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define HNB_CORE_ASSERT(x, ...) { if(!(x)) { HNB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define HNB_ASSERT(x, ...)
#define HNB_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)

#define HNB_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

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