#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/RendererContext.h"

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11Context :public RendererContext
	{
	public:
		DX11Context(HWND* windowHandle);
		void SwapBuffer(bool VSync) override;
		static Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return s_Instance->m_Device; }
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return s_Instance->m_DeviceContext; }
		static Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return s_Instance->m_SwapChain; }
	private:
		void Init();
		HWND* m_WindowHandle;
		static DX11Context* s_Instance;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
	};
}
#endif