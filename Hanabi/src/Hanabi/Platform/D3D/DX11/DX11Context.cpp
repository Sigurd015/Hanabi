#include "hnbpch.h"

#ifdef HNB_ENABLE_D3D
#include "DX11Context.h"

namespace Hanabi
{
	DX11Context::DX11Context(HWND* windowHandle, uint32_t width, uint32_t height, bool vsync)
	{
		HNB_CORE_ASSERT(windowHandle, "Window handle is null!");
		HNB_CORE_ASSERT(!m_SwapChain, "DX11Context already exists!");
		HNB_CORE_ASSERT(!m_Device, "DX11Context already exists!");
		HNB_CORE_ASSERT(!m_DeviceContext, "DX11Context already exists!");

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = *windowHandle;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		DX_CHECK_RESULT(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
			m_SwapChain.GetAddressOf(), m_Device.GetAddressOf(), nullptr, m_DeviceContext.GetAddressOf()));
	}

	void DX11Context::SwapBuffer(bool VSync)
	{
		m_SwapChain->Present(VSync, 0);
	}
}
#endif