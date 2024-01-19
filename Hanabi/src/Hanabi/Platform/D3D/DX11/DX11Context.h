#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/RendererContext.h"
#include "Hanabi/Platform/D3D/DXCommon.h"
#include "Hanabi/Core/Window.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Context :public RendererContext
	{
	public:
		DX11Context(HWND* windowHandle, uint32_t width, uint32_t height, bool vsync);
		void SwapBuffer(bool VSync) override;
		static ComPtr<ID3D11Device> GetDevice() { return m_Device; }
		static ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_DeviceContext; }
		static ComPtr<IDXGISwapChain> GetSwapChain() { return m_SwapChain; }
	private:
		inline static ComPtr<IDXGISwapChain> m_SwapChain;
		inline static ComPtr<ID3D11Device> m_Device;
		inline static ComPtr<ID3D11DeviceContext> m_DeviceContext;
	};
}
#endif