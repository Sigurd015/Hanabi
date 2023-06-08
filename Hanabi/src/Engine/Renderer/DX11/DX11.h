#if defined(HNB_PLATFORM_WINDOWS)

#pragma once
#include <Windows.h>
#include <comdef.h>
#ifdef HNB_DEBUG
#define DX_CHECK_RESULT(x) {if(FAILED(x)){_com_error err(x);LPCTSTR errMsg = err.ErrorMessage();HNB_DEBUGBREAK();}}
#else
#define DX_CHECK_RESULT(X) X
#endif

#endif