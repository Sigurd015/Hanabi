#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "DX11Framebuffer.h"
#include "DX11Context.h"
#include "DX11Framebuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	static const uint32_t s_MaxFramebufferSize = 8192;
	namespace Utils
	{
		static DXGI_FORMAT ImageFormatToDXDSVFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_D32_FLOAT;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

		static bool IsMousePickFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RED8UI:  return true;
			}

			return false;
		}
	}

	DX11Framebuffer::DX11Framebuffer(const FramebufferSpecification& spec) : m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

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
			m_DSAttachmentDSV.Reset();
			if (m_DSAttachment)
				m_DSAttachment->Release();
		}
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
			m_DSAttachmentDSV.Reset();
			if (m_DSAttachment)
				m_DSAttachment->Release();
		}

		if (m_ColorAttachmentSpecifications.size())
		{
			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				ImageSpecification spec = {};
				spec.Width = m_Specification.Width;
				spec.Height = m_Specification.Height;
				spec.Format = m_ColorAttachmentSpecifications[i].TextureFormat;
				spec.Usage = ImageUsage::Attachment;
				Ref<DX11Image2D> image = CreateRef<DX11Image2D>(spec);
				image->Invalidate();

				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
				D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
				targetViewDesc.Format = image->GetFormat();
				targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				targetViewDesc.Texture2D.MipSlice = 0;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRenderTargetView(image->GetTexture().Get(), &targetViewDesc, &targetView));

				m_ColorAttachments.emplace_back(std::move(image));
				m_ColorAttachmentRTV.emplace_back(std::move(targetView));
			}
		}
		if (m_DepthAttachmentSpecification.TextureFormat != ImageFormat::None)
		{
			ImageSpecification spec = {};
			spec.Width = m_Specification.Width;
			spec.Height = m_Specification.Height;
			spec.Format = m_DepthAttachmentSpecification.TextureFormat;
			spec.Usage = ImageUsage::Attachment;
			m_DSAttachment = CreateRef<DX11Image2D>(spec);
			m_DSAttachment->Invalidate();

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = Utils::ImageFormatToDXDSVFormat(m_DepthAttachmentSpecification.TextureFormat);
			depthStencilViewDesc.Flags = 0;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilView(m_DSAttachment->GetTexture().Get(), &depthStencilViewDesc,
				m_DSAttachmentDSV.GetAddressOf()));
		}
	}

	void DX11Framebuffer::Bind()
	{
		DX11Context::GetDeviceContext()->OMSetRenderTargets(m_ColorAttachmentRTV.size(), m_ColorAttachmentRTV.data()->GetAddressOf(),
			m_DSAttachmentDSV.Get());
	}

	void DX11Framebuffer::Unbind()
	{
		DX11Context::GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void DX11Framebuffer::ClearAttachment(const glm::vec4& color)
	{
		for (size_t i = 0; i < m_ColorAttachmentRTV.size(); i++)
		{
			if (Utils::IsMousePickFormat(m_ColorAttachmentSpecifications[i].TextureFormat))
			{
				float temp[4] = { m_Specification.MousePickClearValue,0,0,0 };
				DX11Context::GetDeviceContext()->ClearRenderTargetView(m_ColorAttachmentRTV[i].Get(), temp);
			}
			else
				DX11Context::GetDeviceContext()->ClearRenderTargetView(m_ColorAttachmentRTV[i].Get(), glm::value_ptr(color));
		}
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

	int DX11Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		D3D11_TEXTURE2D_DESC textureDesc = {};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> temp = m_ColorAttachments[attachmentIndex]->GetTexture();
		temp->GetDesc(&textureDesc);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> textureCopy;
		D3D11_TEXTURE2D_DESC textureCopyDesc = textureDesc;
		textureCopyDesc.Usage = D3D11_USAGE_STAGING;
		textureCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureCopyDesc.BindFlags = 0;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureCopyDesc, nullptr, textureCopy.GetAddressOf()));
		DX11Context::GetDeviceContext()->CopyResource(textureCopy.Get(), m_ColorAttachments[attachmentIndex]->GetTexture().Get());

		D3D11_MAPPED_SUBRESOURCE mappedTexture;
		DX_CHECK_RESULT(DX11Context::GetDeviceContext()->Map(textureCopy.Get(), 0, D3D11_MAP_READ, 0, &mappedTexture));

		uint8_t* pData = reinterpret_cast<uint8_t*>(mappedTexture.pData);
		int32_t rowPitch = mappedTexture.RowPitch;
		int32_t pixelValue = -1;

		if (textureDesc.Format == DXGI_FORMAT_R32_SINT)
			memcpy(&pixelValue, pData + y * rowPitch + x * sizeof(int32_t), sizeof(int32_t));

		DX11Context::GetDeviceContext()->Unmap(textureCopy.Get(), 0);
		textureCopy.Reset();

		return pixelValue;
	}
}
#endif