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
		static DXGI_SAMPLE_DESC Multisample(UINT count, DXGI_FORMAT format)
		{
			DXGI_SAMPLE_DESC sampleDesc{};
			sampleDesc.Count = count;
			if (count > 1)
			{
				UINT  m_Quality;
				DX11Context::GetDevice()->CheckMultisampleQualityLevels(format, count, &m_Quality);
				sampleDesc.Quality = m_Quality - 1;
			}
			else
			{
				sampleDesc.Quality = 0;
			}
			return sampleDesc;
		}

		static DXGI_FORMAT FBTextureFormatToDX11(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA8F:       return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case ImageFormat::RED8UI:       return DXGI_FORMAT_R32_SINT;
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24G8_TYPELESS;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_R32_TYPELESS;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		static DXGI_FORMAT DepthStencilViewFormatToDX11(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_D32_FLOAT;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		static DXGI_FORMAT ShadowResourceViewFormatToDX11(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_R32_FLOAT;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		static bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8:  return true;
			case ImageFormat::DEPTH32F:  return true;
			}

			return false;
		}

		static bool IsShadowMap(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH32F:  return true;
			}

			return false;
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
				m_ColorAttachmentTextures[i].Reset();
				m_ColorAttachmentSRV[i].Reset();
			}
			m_ColorAttachmentTextures.clear();
			m_ColorAttachmentRTV.clear();
			m_DepthStencilAttachment.Reset();
			m_DepthStencilAttachmentsTexture.Reset();
			m_ColorAttachmentSRV.clear();
		}
	}

	void DX11Framebuffer::Invalidate()
	{
		if (!m_ColorAttachmentRTV.empty())
		{
			for (size_t i = 0; i < m_ColorAttachmentRTV.size(); i++)
			{
				m_ColorAttachmentRTV[i].Reset();
				m_ColorAttachmentTextures[i].Reset();
				m_ColorAttachmentSRV[i].Reset();
			}
			m_ColorAttachmentTextures.clear();
			m_ColorAttachmentRTV.clear();
			m_DepthStencilAttachment.Reset();
			m_DepthStencilAttachmentsTexture.Reset();
			m_ColorAttachmentSRV.clear();
		}

		if (m_ColorAttachmentSpecifications.size())
		{
			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				Microsoft::WRL::ComPtr<ID3D11Texture2D>  texture;
				D3D11_TEXTURE2D_DESC textureDesc = {};
				textureDesc.Width = m_Specification.Width;
				textureDesc.Height = m_Specification.Height;
				textureDesc.MipLevels = 1;
				textureDesc.ArraySize = 1;
				textureDesc.Format = Utils::FBTextureFormatToDX11(m_ColorAttachmentSpecifications[i].TextureFormat);
				textureDesc.SampleDesc = Utils::Multisample(m_Specification.Samples, textureDesc.Format);
				textureDesc.Usage = D3D11_USAGE_DEFAULT;
				textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				textureDesc.CPUAccessFlags = 0;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf()));
				m_ColorAttachmentTextures.push_back(texture);

				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
				D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
				targetViewDesc.Format = textureDesc.Format;
				targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				targetViewDesc.Texture2D.MipSlice = 0;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRenderTargetView(texture.Get(), &targetViewDesc, &targetView));
				m_ColorAttachmentRTV.push_back(targetView);

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
				shaderResourceDesc.Format = textureDesc.Format;
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceDesc.Texture2D.MipLevels = 1;
				DX11Context::GetDevice()->CreateShaderResourceView(texture.Get(), &shaderResourceDesc, shaderResourceView.GetAddressOf());
				m_ColorAttachmentSRV.push_back(shaderResourceView);
			}
		}
		if (m_DepthAttachmentSpecification.TextureFormat != ImageFormat::None)
		{
			bool isShadowMap = Utils::IsShadowMap(m_DepthAttachmentSpecification.TextureFormat);

			D3D11_TEXTURE2D_DESC depthStencilDesc = {};
			depthStencilDesc.Width = m_Specification.Width;
			depthStencilDesc.Height = m_Specification.Height;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = Utils::FBTextureFormatToDX11(m_DepthAttachmentSpecification.TextureFormat);
			depthStencilDesc.SampleDesc = Utils::Multisample(m_Specification.Samples, depthStencilDesc.Format);
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			if (isShadowMap)
				depthStencilDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

			depthStencilDesc.CPUAccessFlags = 0;
			depthStencilDesc.MiscFlags = 0;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr,
				m_DepthStencilAttachmentsTexture.GetAddressOf()));

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = Utils::DepthStencilViewFormatToDX11(m_DepthAttachmentSpecification.TextureFormat);
			depthStencilViewDesc.Flags = 0;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilView(m_DepthStencilAttachmentsTexture.Get(), &depthStencilViewDesc,
				m_DepthStencilAttachment.GetAddressOf()));

			if (isShadowMap)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
				shaderResourceDesc.Format = Utils::ShadowResourceViewFormatToDX11(m_DepthAttachmentSpecification.TextureFormat);
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceDesc.Texture2D.MipLevels = 1;
				DX11Context::GetDevice()->CreateShaderResourceView(m_DepthStencilAttachmentsTexture.Get(), &shaderResourceDesc,
					m_DepthStencilSRV.GetAddressOf());
			}
		}
	}

	void DX11Framebuffer::Bind()
	{
		DX11Context::GetDeviceContext()->OMSetRenderTargets(m_ColorAttachmentRTV.size(), m_ColorAttachmentRTV.data()->GetAddressOf(),
			m_DepthStencilAttachment.Get());
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
		DX11Context::GetDeviceContext()->ClearDepthStencilView(m_DepthStencilAttachment.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			m_Specification.DepthClearValue, 0);
	}

	void DX11Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			HNB_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		if (width == m_Specification.Width && height == m_Specification.Height)
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;

		D3D11_VIEWPORT viewPort{};
		viewPort.Width = width;
		viewPort.Height = height;
		viewPort.MinDepth = 0;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		DX11Context::GetDeviceContext()->RSSetViewports(1, &viewPort);

		Invalidate();
	}

	int DX11Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachmentTextures.size());

		D3D11_TEXTURE2D_DESC textureDesc = {};
		m_ColorAttachmentTextures[attachmentIndex]->GetDesc(&textureDesc);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> textureCopy;
		D3D11_TEXTURE2D_DESC textureCopyDesc = textureDesc;
		textureCopyDesc.Usage = D3D11_USAGE_STAGING;
		textureCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureCopyDesc.BindFlags = 0;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureCopyDesc, nullptr, textureCopy.GetAddressOf()));
		DX11Context::GetDeviceContext()->CopyResource(textureCopy.Get(), m_ColorAttachmentTextures[attachmentIndex].Get());

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

	void* DX11Framebuffer::GetColorAttachment(uint32_t index) const
	{
		HNB_CORE_ASSERT(index < m_ColorAttachmentSRV.size());

		return m_ColorAttachmentSRV[index].Get();
	}

	void* DX11Framebuffer::GetDepthAttachment() const
	{
		HNB_CORE_ASSERT(Utils::IsShadowMap(m_DepthAttachmentSpecification.TextureFormat));

		return m_DepthStencilSRV.Get();
	}
}
#endif