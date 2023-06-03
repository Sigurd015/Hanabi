#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Texture.h"
#include "Engine/Renderer/DX11/DX11Context.h"
#include "Engine/Core/UUID.h"

#include <stb_image.h>

namespace Hanabi
{
	void CreateTexDesc(D3D11_USAGE usage, int cpuAccess, uint32_t width, uint32_t height,
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
		HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, pInitialData, ppTexture2D));
	}

	void CreateShaderView(DXGI_FORMAT format, ID3D11Resource* pResource, ID3D11ShaderResourceView** ppSRView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
		resourceView.Format = format;
		resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceView.Texture2D.MostDetailedMip = 0;
		resourceView.Texture2D.MipLevels = 1;
		HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateShaderResourceView(pResource, &resourceView, ppSRView));
	}

	void CreateSamplerState(ID3D11SamplerState** ppSamplerState)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, ppSamplerState));
	}

	DX11Texture2D::DX11Texture2D(const std::string& path) : m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		if (data)
		{
			m_IsLoaded = true;
			m_Width = width;
			m_Height = height;
			m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = data;
			subresourceData.SysMemPitch = m_Width * 4;
			CreateTexDesc(D3D11_USAGE_DEFAULT, 0, m_Width, m_Height, m_DataFormat, &subresourceData, m_Texture.GetAddressOf());
			CreateShaderView(m_DataFormat, m_Texture.Get(), m_TextureView.GetAddressOf());
			CreateSamplerState(m_SamplerState.GetAddressOf());
			stbi_image_free(data);
		}
	}

	DX11Texture2D::DX11Texture2D(const TextureSpecification& specification) :m_Specification(specification),
		m_Width(specification.Width), m_Height(specification.Height)
	{
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		CreateTexDesc(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, m_Width, m_Height, m_DataFormat, nullptr, m_Texture.GetAddressOf());
		CreateShaderView(m_DataFormat, m_Texture.Get(), m_TextureView.GetAddressOf());
		CreateSamplerState(m_SamplerState.GetAddressOf());
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_Texture.Reset();
		m_TextureView.Reset();
		m_SamplerState.Reset();
	}

	void DX11Texture2D::SetData(void* data, uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HNB_CORE_DX_ASSERT(DX11Context::GetDeviceContext()->Map(m_Texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		if (m_Specification.Format == ImageFormat::RGB8)
		{
			uint8_t* targetData = static_cast<uint8_t*>(mappedResource.pData);
			uint8_t* srcData = static_cast<uint8_t*>(data);

			// Set Alpha to 255	
			for (uint32_t i = 0; i < size / 3; i++)
			{
				targetData[i * 4] = srcData[i * 3];     // R
				targetData[i * 4 + 1] = srcData[i * 3 + 1]; // G
				targetData[i * 4 + 2] = srcData[i * 3 + 2]; // B
				targetData[i * 4 + 3] = 255;           // Alpha
			}
		}
		else
			memcpy(mappedResource.pData, data, size);

		DX11Context::GetDeviceContext()->Unmap(m_Texture.Get(), 0);
	}

	bool DX11Texture2D::operator==(const Texture& other) const
	{
		ID3D11Resource* resource1;
		ID3D11Resource* resource2;
		m_TextureView->GetResource(&resource1);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> temp = static_cast<ID3D11ShaderResourceView*>(other.GetRendererID());
		temp->GetResource(&resource2);
		return resource1 == resource2;
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, m_SamplerState.GetAddressOf());
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureView.GetAddressOf());
	}
}
#endif