#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)

namespace Hanabi
{
	bool CheckD3DError(HRESULT result)
	{
		if (result == S_OK)
			return true;

		switch (result)
		{
		case D3D11_ERROR_FILE_NOT_FOUND:
			HNB_CORE_ERROR("D3D-ErrorType:D3D11_ERROR_FILE_NOT_FOUND");
			return false;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			HNB_CORE_ERROR("D3D-ErrorType:D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS");
			return false;
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			HNB_CORE_ERROR("D3D-ErrorType:D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS");
			return false;
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			HNB_CORE_ERROR("D3D-ErrorType:D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD");
			return false;
		case DXGI_ERROR_INVALID_CALL:
			HNB_CORE_ERROR("D3D-ErrorType:DXGI_ERROR_INVALID_CALL");
			return false;
		case DXGI_ERROR_WAS_STILL_DRAWING:
			HNB_CORE_ERROR("D3D-ErrorType:DXGI_ERROR_WAS_STILL_DRAWING");
			return false;
		case E_FAIL:
			HNB_CORE_ERROR("D3D-ErrorType:E_FAIL");
			return false;
		case E_INVALIDARG:
			HNB_CORE_ERROR("D3D-ErrorType:E_INVALIDARG");
			return false;
		case E_OUTOFMEMORY:
			HNB_CORE_ERROR("D3D-ErrorType:E_OUTOFMEMORY");
			return false;
		case E_NOTIMPL:
			HNB_CORE_ERROR("D3D-ErrorType:E_NOTIMPL");
			return false;
		case S_FALSE:
			HNB_CORE_ERROR("D3D-ErrorType:S_FALSE");
			return false;
		}
	}
}
#endif