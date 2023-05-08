#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Framebuffer.h"

namespace Hanabi
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	DX11Framebuffer::DX11Framebuffer(const FramebufferSpecification& spec) : m_Specification(spec)
	{

	}

	DX11Framebuffer::~DX11Framebuffer()
	{

	}

	void DX11Framebuffer::Invalidate()
	{

	}

	void DX11Framebuffer::Bind()
	{

	}

	void DX11Framebuffer::Unbind()
	{

	}

	void DX11Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int DX11Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		return 1;
	}

	void DX11Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{

	}
}
#endif