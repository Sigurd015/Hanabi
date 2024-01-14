#ifdef HNB_ENABLE_D3D

#pragma once
#include "Hanabi/Core/Log.h"
#include "Hanabi/Core/Assert.h"

#include <Windows.h>
#include <comdef.h>
#include <d3dcompiler.h>
#include <wrl.h>

namespace Hanabi
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	namespace Utils
	{
		inline const char* DXResultToString(HRESULT result)
		{
			switch (result)
			{
			case D3D11_ERROR_FILE_NOT_FOUND: return "D3D11_ERROR_FILE_NOT_FOUND";
			case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
			case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
			case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: return "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
			case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
			case DXGI_ERROR_WAS_STILL_DRAWING: return "DXGI_ERROR_WAS_STILL_DRAWING";
			case E_FAIL: return "E_FAIL";
			case E_INVALIDARG: return "E_INVALIDARG";
			case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
			case E_NOTIMPL: return "E_NOTIMPL";
			case S_FALSE: return "S_FALSE";
			}
			HNB_CORE_ASSERT(false);
			return nullptr;
		}

		inline void DXCheckResult(HRESULT result, const char* file, int line)
		{
			if (result != S_OK)
			{
				HNB_CORE_ERROR("DXResult is '{0}' in {1}:{2}", Utils::DXResultToString(result), file, line);
				HNB_CORE_ASSERT(result == S_OK);
			}
		}
	}
}

#ifndef HNB_DIST
#define DX_CHECK_RESULT(f)\
{\
HRESULT res = (f); \
::Hanabi::Utils::DXCheckResult(res, __FILE__, __LINE__); \
}
#else
#define DX_CHECK_RESULT(f) f
#endif

#endif