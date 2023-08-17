#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11Texture.h"
#include "DX11Context.h"
#include "Engine/Core/UUID.h"
#include "Engine/Platform/D3D/DXCommon.h"
#include "DX11RenderStates.h"

#include <stb_image.h>

namespace Hanabi
{
	namespace Utils
	{
		static DXGI_FORMAT ImageFormatToDXDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return DXGI_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	void CreateTexture(D3D11_USAGE usage, int cpuAccess, uint32_t width, uint32_t height,
		DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
	{
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = usage;
		textureDesc.CPUAccessFlags = cpuAccess;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = 0;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, pInitialData, ppTexture2D));
	}

	void CreateShaderView(DXGI_FORMAT format, ID3D11Resource* pResource, ID3D11ShaderResourceView** ppSRView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
		resourceView.Format = format;
		resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceView.Texture2D.MostDetailedMip = 0;
		resourceView.Texture2D.MipLevels = 1;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(pResource, &resourceView, ppSRView));
	}

	DX11Texture2D::DX11Texture2D(const TextureSpecification& specification, Buffer data) :m_Specification(specification),
		m_Width(specification.Width), m_Height(specification.Height)
	{
		m_DataFormat = Utils::ImageFormatToDXDataFormat(specification.Format);
		if (!data.Size)
		{
			CreateTexture(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, m_Width, m_Height, m_DataFormat, nullptr, m_Texture.GetAddressOf());
		}
		else
		{
			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = data.Data;
			subresourceData.SysMemPitch = m_Width * 4;  // size of one row in bytes
			CreateTexture(D3D11_USAGE_DEFAULT, 0, m_Width, m_Height, m_DataFormat, &subresourceData, m_Texture.GetAddressOf());
		}
		CreateShaderView(m_DataFormat, m_Texture.Get(), m_TextureSRV.GetAddressOf());
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_Texture.Reset();
		m_TextureSRV.Reset();
	}

	void DX11Texture2D::SetData(Buffer data)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DX_CHECK_RESULT(DX11Context::GetDeviceContext()->Map(m_Texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		if (m_Specification.Format == ImageFormat::RGB8)
		{
			uint8_t* targetData = static_cast<uint8_t*>(mappedResource.pData);
			uint8_t* srcData = static_cast<uint8_t*>(data.Data);

			// Set Alpha to 255	
			for (uint32_t i = 0; i < data.Size / 3; i++)
			{
				targetData[i * 4] = srcData[i * 3];     // R
				targetData[i * 4 + 1] = srcData[i * 3 + 1]; // G
				targetData[i * 4 + 2] = srcData[i * 3 + 2]; // B
				targetData[i * 4 + 3] = 255;           // Alpha
			}
		}
		else
			memcpy(mappedResource.pData, data.Data, data.Size);

		DX11Context::GetDeviceContext()->Unmap(m_Texture.Get(), 0);
	}

	bool DX11Texture2D::operator==(const Texture& other) const
	{
		ID3D11Resource* resource1;
		ID3D11Resource* resource2;
		m_TextureSRV->GetResource(&resource1);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> temp = static_cast<ID3D11ShaderResourceView*>(other.GetRendererID());
		temp->GetResource(&resource2);
		return resource1 == resource2;
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		switch (m_Specification.SamplerFilter)
		{
		case TextureFilter::Linear:
		{
			if (m_Specification.SamplerWrap == TextureWrap::Repeat)
				DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, DX11RenderStates::SSLinearWrap.GetAddressOf());

			if (m_Specification.SamplerWrap == TextureWrap::Clamp)
				DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, DX11RenderStates::SSLinearClamp.GetAddressOf());

			break;
		}
		}
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureSRV.GetAddressOf());
	}
}
#endif