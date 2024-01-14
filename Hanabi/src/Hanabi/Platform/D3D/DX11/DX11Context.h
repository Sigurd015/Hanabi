#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/RendererContext.h"
#include "Hanabi/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Context :public RendererContext
	{
	public:
		DX11Context(HWND* windowHandle);
		void SwapBuffer(bool VSync) override;
		static ComPtr<ID3D11Device> GetDevice() { return s_Instance->m_Device; }
		static ComPtr<ID3D11DeviceContext> GetDeviceContext() { return s_Instance->m_DeviceContext; }
		static ComPtr<IDXGISwapChain> GetSwapChain() { return s_Instance->m_SwapChain; }
	private:
		void Init();
		HWND* m_WindowHandle;
		static DX11Context* s_Instance;
		ComPtr<IDXGISwapChain> m_SwapChain;
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_DeviceContext;
	};
}
#endif