#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Context.h"

namespace Hanabi
{
	DX11Context* DX11Context::s_Instance = nullptr;

	DX11Context::DX11Context(HWND* windowHandle) :m_WindowHandle(windowHandle)
	{
		HNB_CORE_ASSERT(windowHandle, "Window handle is null!");

		Init();
	}

	void DX11Context::Init()
	{
		HNB_CORE_ASSERT(!s_Instance, "DX11Context already exists!");

		s_Instance = this;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = *m_WindowHandle;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		HNB_CORE_DX_ASSERT(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
			m_SwapChain.GetAddressOf(), m_Device.GetAddressOf(), nullptr, m_DeviceContext.GetAddressOf()));
	}

	void DX11Context::SwapBuffer(bool VSync)
	{
		m_SwapChain->Present(VSync, 0);
	}
}
#endif