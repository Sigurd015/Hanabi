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
