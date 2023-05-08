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
		DX11Context::GetDevice()->CreateTexture2D(&textureDesc, pInitialData, ppTexture2D);
	}

	void CreateShaderView(DXGI_FORMAT format, ID3D11Resource* pResource, ID3D11ShaderResourceView** ppSRView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
		resourceView.Format = format;
		resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceView.Texture2D.MostDetailedMip = 0;
		resourceView.Texture2D.MipLevels = 1;
		DX11Context::GetDevice()->CreateShaderResourceView(pResource, &resourceView, ppSRView);
	}

	void CreateSamplerState(ID3D11SamplerState** ppSamplerState)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, ppSamplerState);
	}

	DX11Texture2D::DX11Texture2D(const std::string& path) : m_Path(path), m_UUID(UUID())
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

	DX11Texture2D::DX11Texture2D(uint32_t width, uint32_t height) : m_Width(width), m_Height(height), m_UUID(UUID())
	{
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		CreateTexDesc(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, m_Width, m_Height, m_DataFormat, nullptr, m_Texture.GetAddressOf());
		CreateShaderView(m_DataFormat, m_Texture.Get(), m_TextureView.GetAddressOf());
		CreateSamplerState(m_SamplerState.GetAddressOf());
	}

	DX11Texture2D::~DX11Texture2D()
	{}

	void DX11Texture2D::SetData(void* data, uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DX11Context::GetDeviceContext()->Map(m_Texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, data, size);
		DX11Context::GetDeviceContext()->Unmap(m_Texture.Get(), 0);
	}

	bool DX11Texture2D::operator==(const Texture& other) const
	{
		return m_UUID == other.GetRendererID();
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, m_SamplerState.GetAddressOf());
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureView.GetAddressOf());
	}
}
#endif