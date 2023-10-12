#include "hnbpch.h"
#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11Image.h"
#include "DX11Context.h"

namespace Hanabi
{
	namespace Utils
	{
		// UNORM: Unsigned normalized
		// SNORM: Signed normalized
		// FLOAT: Signed float
		static DXGI_FORMAT ImageFormatToDXTextureFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case ImageFormat::RGBA8:           return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case ImageFormat::RGBA8F:          return DXGI_FORMAT_R8G8B8A8_SNORM;
			case ImageFormat::RGBA16F:         return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case ImageFormat::RGBA32F:         return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24G8_TYPELESS;
			case ImageFormat::DEPTH32F:        return DXGI_FORMAT_R32_TYPELESS;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

		static DXGI_FORMAT ImageFormatToDXDepthSRVFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_R32_FLOAT;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

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
	}

	DX11Image2D::DX11Image2D(const ImageSpecification& specification, Buffer buffer) :m_Specification(specification)
	{
		m_DataFormat = Utils::ImageFormatToDXTextureFormat(m_Specification.Format);
		m_ImageData = Buffer::Copy(buffer);
	}

	DX11Image2D::~DX11Image2D()
	{
		Release();
	}

	void DX11Image2D::Resize(const uint32_t width, const uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
	}

	void DX11Image2D::Invalidate()
	{
		switch (m_Specification.Usage)
		{
		case ImageUsage::Attachment:
		{
			bool isdepth = Utils::IsDepthFormat(m_Specification.Format);

			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_DataFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.CPUAccessFlags = 0;
			if (isdepth)
			{
				textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			}
			else
			{
				textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			}
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, m_Texture.GetAddressOf()));

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
			shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceDesc.Texture2D.MipLevels = 1;
			if (isdepth)
			{
				shaderResourceDesc.Format = Utils::ImageFormatToDXDepthSRVFormat(m_Specification.Format);
			}
			else
			{
				shaderResourceDesc.Format = textureDesc.Format;
			}
			DX11Context::GetDevice()->CreateShaderResourceView(m_Texture.Get(), &shaderResourceDesc, m_TextureSRV.GetAddressOf());
			break;
		}
		case ImageUsage::Texture2D:
		{
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_DataFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.MiscFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = m_ImageData.Data;
			subresourceData.SysMemPitch = m_Specification.Width * 4;  // size of one row in bytes
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, &subresourceData, m_Texture.GetAddressOf()));

			D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
			resourceView.Format = m_DataFormat;
			resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			resourceView.Texture2D.MostDetailedMip = 0;
			resourceView.Texture2D.MipLevels = 1;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(m_Texture.Get(), &resourceView, m_TextureSRV.GetAddressOf()));
			break;
		}
		case ImageUsage::TextureCube:
		{
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 6;
			textureDesc.Format = m_DataFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.CPUAccessFlags = 0;
			uint32_t faceSize = m_ImageData.Size / 6;
			D3D11_SUBRESOURCE_DATA subresourceData[6] = {};
			for (size_t i = 0; i < 6; i++)
			{
				subresourceData[i].pSysMem = m_ImageData.Data + i * faceSize;
				subresourceData[i].SysMemPitch = m_Specification.Width * 4;  // size of one row in bytes
			}
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, subresourceData, m_Texture.GetAddressOf()));

			D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
			resourceView.Format = m_DataFormat;
			resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			resourceView.Texture2D.MostDetailedMip = 0;
			resourceView.Texture2D.MipLevels = 1;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(m_Texture.Get(), &resourceView, m_TextureSRV.GetAddressOf()));
			break;
		}
		}
	}

	void DX11Image2D::Release()
	{
		m_Texture.Reset();
		m_TextureSRV.Reset();
	}

	void DX11Image2D::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureSRV.GetAddressOf());
	}
}
#endif