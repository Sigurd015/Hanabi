#include "hnbpch.h"
#include "Engine/Renderer/DX11/DX11API.h"
#include "Engine/Renderer/DX11/DX11Framebuffer.h"
#include "Engine/Renderer/DX11/DX11Context.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Framebuffer.h"

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

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
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
		if (!m_RenderTargetAttachments.empty())
		{
			for (size_t i = 0; i < m_RenderTargetAttachments.size(); i++)
			{
				m_RenderTargetAttachments[i].Reset();
				m_RenderTargetAttachmentsTextures[i].Reset();
				m_ShaderResourceViews[i].Reset();
			}
			m_RenderTargetAttachmentsTextures.clear();
			m_RenderTargetAttachments.clear();
			m_DepthStencilAttachment.Reset();
			m_DepthStencilAttachmentsTexture.Reset();
			m_ShaderResourceViews.clear();
		}
	}

	void DX11Framebuffer::Invalidate()
	{
		if (!m_RenderTargetAttachments.empty())
		{
			for (size_t i = 0; i < m_RenderTargetAttachments.size(); i++)
			{
				m_RenderTargetAttachments[i].Reset();
				m_RenderTargetAttachmentsTextures[i].Reset();
				m_ShaderResourceViews[i].Reset();
			}
			m_RenderTargetAttachmentsTextures.clear();
			m_RenderTargetAttachments.clear();
			m_DepthStencilAttachment.Reset();
			m_DepthStencilAttachmentsTexture.Reset();
			m_ShaderResourceViews.clear();
		}

		if (m_ColorAttachmentSpecifications.size())
		{
			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				Microsoft::WRL::ComPtr<ID3D11Texture2D>  texture;
				D3D11_TEXTURE2D_DESC textureDesc = {};
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
				{
					textureDesc.Width = m_Specification.Width;
					textureDesc.Height = m_Specification.Height;
					textureDesc.MipLevels = 1;
					textureDesc.ArraySize = 1;
					textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					textureDesc.SampleDesc = Utils::Multisample(m_Specification.Samples, DXGI_FORMAT_R32G32B32A32_FLOAT);
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;
					HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf()))
						break;
				}
				case FramebufferTextureFormat::RED_INTEGER:
				{
					textureDesc.Width = m_Specification.Width;
					textureDesc.Height = m_Specification.Height;
					textureDesc.MipLevels = 1;
					textureDesc.ArraySize = 1;
					textureDesc.Format = DXGI_FORMAT_R32_SINT;
					textureDesc.SampleDesc = Utils::Multisample(m_Specification.Samples, DXGI_FORMAT_R32_SINT);
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;
					HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf()))
						break;
				}
				}
				m_RenderTargetAttachmentsTextures.push_back(texture);
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
				D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
				targetViewDesc.Format = textureDesc.Format;
				targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				targetViewDesc.Texture2D.MipSlice = 0;
				HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateRenderTargetView(texture.Get(), &targetViewDesc, &targetView));
				m_RenderTargetAttachments.push_back(targetView);

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
				shaderResourceDesc.Format = textureDesc.Format;
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceDesc.Texture2D.MipLevels = 1;
				DX11Context::GetDevice()->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());
				m_ShaderResourceViews.push_back(shaderResourceView);
			}
		}
		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
			{
				D3D11_TEXTURE2D_DESC depthStencilDesc = {};
				depthStencilDesc.Width = m_Specification.Width;
				depthStencilDesc.Height = m_Specification.Height;
				depthStencilDesc.MipLevels = 1;
				depthStencilDesc.ArraySize = 1;
				depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilDesc.SampleDesc = Utils::Multisample(m_Specification.Samples, DXGI_FORMAT_D24_UNORM_S8_UINT);
				depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
				depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthStencilDesc.CPUAccessFlags = 0;
				depthStencilDesc.MiscFlags = 0;
				HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, m_DepthStencilAttachmentsTexture.GetAddressOf()));
				HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateDepthStencilView(m_DepthStencilAttachmentsTexture.Get(), nullptr, m_DepthStencilAttachment.GetAddressOf()));
				break;
			}
			}
		}
	}

	void DX11Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		const glm::vec4 color = DX11RendererAPI::GetClearColor();
		for (size_t i = 0; i < m_RenderTargetAttachments.size(); i++)
		{
			if (i == attachmentIndex)
			{
				const float temp[] = { value,0,0,0 };
				DX11Context::GetDeviceContext()->ClearRenderTargetView(m_RenderTargetAttachments[i].Get(), temp);
				continue;
			}
			DX11Context::GetDeviceContext()->ClearRenderTargetView(m_RenderTargetAttachments[i].Get(), &color.x);
		}
		DX11Context::GetDeviceContext()->ClearDepthStencilView(m_DepthStencilAttachment.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		DX11Context::GetDeviceContext()->OMSetRenderTargets(m_RenderTargetAttachments.size(), m_RenderTargetAttachments.data()->GetAddressOf(), m_DepthStencilAttachment.Get());
	}

	void DX11Framebuffer::Bind()
	{
		DX11RendererAPI::Get()->SetAttachments();
	}

	void DX11Framebuffer::Unbind()
	{
		DX11RendererAPI::Get()->ReSetAttachments();
	}

	void DX11Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int DX11Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		m_RenderTargetAttachmentsTextures[attachmentIndex]->GetDesc(&textureDesc);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> textureCopy;
		D3D11_TEXTURE2D_DESC textureCopyDesc = textureDesc;
		textureCopyDesc.Usage = D3D11_USAGE_STAGING;
		textureCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateTexture2D(&textureCopyDesc, nullptr, &textureCopy));
		DX11Context::GetDeviceContext()->CopyResource(textureCopy.Get(), m_RenderTargetAttachmentsTextures[attachmentIndex].Get());

		D3D11_MAPPED_SUBRESOURCE mappedTexture;
		HNB_CORE_DX_ASSERT(DX11Context::GetDeviceContext()->Map(textureCopy.Get(), 0, D3D11_MAP_READ, 0, &mappedTexture));

		uint32_t* id = static_cast<uint32_t*>(mappedTexture.pData);
		uint32_t objectId = id[x + (m_Specification.Height - y - 1) * m_Specification.Width];

		DX11Context::GetDeviceContext()->Unmap(textureCopy.Get(), 0);

		textureCopy->Release();

		return objectId;
	}

	void* DX11Framebuffer::GetColorAttachment(uint32_t index) const
	{
		return m_ShaderResourceViews[index].Get();
	}
}
#endif