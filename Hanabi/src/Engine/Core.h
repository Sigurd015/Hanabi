#pragma once

#ifdef HNB_PLATFORM_WINDOWS
	#ifdef HNB_BUILD_DLL
		#define HNB_API __declspec(dllexport)
	#else
		#define HNB_API __declspec(dllimport)
	#endif
#else
	#error Only supports windows for now!
#endif 

#ifdef HNB_ENABLE_ASSERTS
#define HNB_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define HNB_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define HNB_ASSERT(x, ...)
#define HNB_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)
