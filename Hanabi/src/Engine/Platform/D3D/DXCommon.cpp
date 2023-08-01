#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)

namespace Hanabi
{
	void CheckD3DError(HRESULT result)
	{
		if (result == S_OK)
			return;

		switch (result)
		{
		case D3D11_ERROR_FILE_NOT_FOUND:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:D3D11_ERROR_FILE_NOT_FOUND");
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS");
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS");
			break;
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD");
			break;
		case DXGI_ERROR_INVALID_CALL:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:DXGI_ERROR_INVALID_CALL");
			break;
		case DXGI_ERROR_WAS_STILL_DRAWING:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:DXGI_ERROR_WAS_STILL_DRAWING");
			break;
		case E_FAIL:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:E_FAIL");
			break;
		case E_INVALIDARG:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:E_INVALIDARG");
			break;
		case E_OUTOFMEMORY:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:E_OUTOFMEMORY");
			break;
		case E_NOTIMPL:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:E_NOTIMPL");
			break;
		case S_FALSE:
			HNB_CORE_ASSERT(false, "D3D-ErrorType:S_FALSE");
			break;
		}
	}
}
#endif