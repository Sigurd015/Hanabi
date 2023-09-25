#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "DX11Framebuffer.h"
#include "DX11Context.h"
#include "DX11Framebuffer.h"

#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	static const uint32_t s_MaxFramebufferSize = 8192;
	namespace Utils
	{
		static DXGI_FORMAT ImageFormatToDXDepthDSVFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_D32_FLOAT;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	DX11Framebuffer::DX11Framebuffer(const FramebufferSpecification& spec) : m_Specification(spec)
	{
		Invalidate();
	}

	DX11Framebuffer::~DX11Framebuffer()
	{
		if (!m_ColorAttachmentRTV.empty())
		{
			for (size_t i = 0; i < m_ColorAttachmentRTV.size(); i++)
			{
				m_ColorAttachmentRTV[i].Reset();
			}
			m_ColorAttachments.clear();
			m_ColorAttachmentRTV.clear();
		}
		m_DSAttachmentDSV.Reset();
	}

	void DX11Framebuffer::Invalidate()
	{
		if (!m_ColorAttachmentRTV.empty())
		{
			for (size_t i = 0; i < m_ColorAttachmentRTV.size(); i++)
			{
				m_ColorAttachmentRTV[i].Reset();
			}
			m_ColorAttachments.clear();
			m_ColorAttachmentRTV.clear();
		}
		m_DSAttachmentDSV.Reset();

		uint32_t attachmentIndex = 0;
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
			{
				Ref<DX11Image2D> image = nullptr;
				if (m_Specification.ExistingImages.find(attachmentIndex) != m_Specification.ExistingImages.end())
				{
					image = std::static_pointer_cast<DX11Image2D>(m_Specification.ExistingImages.at(attachmentIndex));
				}
				else
				{
					ImageSpecification imageSpec = {};
					imageSpec.Width = m_Specification.Width;
					imageSpec.Height = m_Specification.Height;
					imageSpec.Format = spec.TextureFormat;
					imageSpec.Usage = ImageUsage::Attachment;
					image = CreateRef<DX11Image2D>(imageSpec);
					image->Invalidate();
				}

				ComPtr<ID3D11RenderTargetView> targetView;
				D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
				targetViewDesc.Format = image->GetFormat();
				targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				targetViewDesc.Texture2D.MipSlice = 0;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRenderTargetView(image->GetTexture().Get(), &targetViewDesc, &targetView));

				m_ColorAttachments.emplace_back(std::move(image));
				m_ColorAttachmentRTV.emplace_back(std::move(targetView));
			}
			else
			{
				Ref<DX11Image2D> image = nullptr;
				if (m_Specification.ExistingImages.find(attachmentIndex) != m_Specification.ExistingImages.end())
				{
					image = std::static_pointer_cast<DX11Image2D>(m_Specification.ExistingImages.at(attachmentIndex));
				}
				else
				{
					ImageSpecification depthImageSpec = {};
					depthImageSpec.Width = m_Specification.Width;
					depthImageSpec.Height = m_Specification.Height;
					depthImageSpec.Format = spec.TextureFormat;
					depthImageSpec.Usage = ImageUsage::Attachment;
					image = CreateRef<DX11Image2D>(depthImageSpec);
					image->Invalidate();
				}

				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
				depthStencilViewDesc.Format = Utils::ImageFormatToDXDepthDSVFormat(spec.TextureFormat);
				depthStencilViewDesc.Flags = 0;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Texture2D.MipSlice = 0;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilView(image->GetTexture().Get(), &depthStencilViewDesc,
					m_DSAttachmentDSV.GetAddressOf()));

				m_DSAttachment = std::move(image);
			}
			attachmentIndex++;
		}
	}

	void DX11Framebuffer::Bind()
	{
		DX11Context::GetDeviceContext()->OMSetRenderTargets(m_ColorAttachmentRTV.size(), m_ColorAttachmentRTV.data()->GetAddressOf(),
			m_DSAttachmentDSV.Get());
	}

	void DX11Framebuffer::ClearAttachment()
	{
		for (size_t i = 0; i < m_ColorAttachmentRTV.size(); i++)
		{
			DX11Context::GetDeviceContext()->ClearRenderTargetView(m_ColorAttachmentRTV[i].Get(), glm::value_ptr(m_Specification.ClearColor));
		}
		DX11Context::GetDeviceContext()->ClearDepthStencilView(m_DSAttachmentDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			m_Specification.DepthClearValue, 0);
	}

	void DX11Framebuffer::ClearAttachment(uint32_t attachmentIndex)
	{
		HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachmentRTV.size(), "DX11Framebuffer::ClearAttachment - Invalid attachment index!");

		DX11Context::GetDeviceContext()->ClearRenderTargetView(m_ColorAttachmentRTV[attachmentIndex].Get(), glm::value_ptr(m_Specification.ClearColor));
	}

	void DX11Framebuffer::ClearDepthAttachment()
	{
		DX11Context::GetDeviceContext()->ClearDepthStencilView(m_DSAttachmentDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			m_Specification.DepthClearValue, 0);
	}

	void DX11Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			HNB_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}
}
#endif