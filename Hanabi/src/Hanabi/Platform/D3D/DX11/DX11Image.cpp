#include "hnbpch.h"
#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11Image.h"
#include "DX11Context.h"

namespace Hanabi
{
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
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = m_Specification.Layers;
			textureDesc.Format = m_DataFormat;
			// TODO: Implement multisampling
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.CPUAccessFlags = 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};

			if (textureDesc.ArraySize > 1)
			{
				if (textureDesc.ArraySize == 6)
				{
					textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
					shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					shaderResourceDesc.TextureCube.MipLevels = 1;
				}
				else
				{
					shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					shaderResourceDesc.Texture2DArray.MipLevels = 1;
				}
			}
			else
			{
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceDesc.Texture2D.MipLevels = 1;
			}

			if (Utils::IsDepthFormat(m_Specification.Format))
			{
				// If it's a depth attachment, we need to create a depth stencil view and usa a depth stencil format
				textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

				shaderResourceDesc.Format = Utils::ImageFormatToDXDepthSRVFormat(m_Specification.Format);
			}
			else
			{
				// If it's a color attachment, we need to create a render target view and use a regular texture format
				textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

				shaderResourceDesc.Format = textureDesc.Format;
			}
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, m_Texture.GetAddressOf()));

			DX11Context::GetDevice()->CreateShaderResourceView(m_Texture.Get(), &shaderResourceDesc, m_TextureSRV.GetAddressOf());
			break;
		}
		case ImageUsage::Texture2D:
		{
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = m_Specification.Mips;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_DataFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.CPUAccessFlags = 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
			resourceView.Format = m_DataFormat;
			resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			resourceView.Texture2D.MostDetailedMip = 0;

			// Notice: Must sure load image data as RGBA 4 channels
			const static uint32_t defalutDataSize = 4;
			if (m_Specification.Mips != 1)
			{
				// If we have mipmaps, we need to create the texture without data and then update the first mip level with the data
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, m_Texture.GetAddressOf()));
				DX11Context::GetDeviceContext()->UpdateSubresource(m_Texture.Get(), 0, nullptr,
					m_ImageData.Data, m_Specification.Width * defalutDataSize, 0);

				resourceView.Texture2D.MipLevels = -1;
			}
			else
			{
				// If we have no mipmaps, we can directly create the texture with the data
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				textureDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pSysMem = m_ImageData.Data;

				// Notice: RGBA32F format used for HDR image
				if (m_Specification.Format == ImageFormat::RGBA32F)
					subresourceData.SysMemPitch = m_Specification.Width * defalutDataSize * sizeof(float);  // size of one row in bytes
				else
					subresourceData.SysMemPitch = m_Specification.Width * defalutDataSize;  // size of one row in bytes

				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, &subresourceData, m_Texture.GetAddressOf()));

				resourceView.Texture2D.MipLevels = 1;
			}

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

	void DX11Image2D::Bind(const ShaderResourceDeclaration& declaration) const
	{
		switch (declaration.Stage)
		{
		case VertexShader:
			DX11Context::GetDeviceContext()->VSSetShaderResources(declaration.Slot, 1, m_TextureSRV.GetAddressOf());
			break;
		case GeometryShader:
			DX11Context::GetDeviceContext()->GSSetShaderResources(declaration.Slot, 1, m_TextureSRV.GetAddressOf());
			break;
		case PixelShader:
			DX11Context::GetDeviceContext()->PSSetShaderResources(declaration.Slot, 1, m_TextureSRV.GetAddressOf());
			break;
		case ComputeShader:
			DX11Context::GetDeviceContext()->CSSetShaderResources(declaration.Slot, 1, m_TextureSRV.GetAddressOf());
			break;
		default:
			HNB_CORE_ASSERT(false, "Unknown shader stage!");
			break;
		}
	}
}
#endif