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

		void CreateTexture(D3D11_USAGE usage, UINT cpuAccess, UINT miscFlag, uint32_t width, uint32_t height,
			DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
		{
			D3D11_TEXTURE2D_DESC textureDesc = { 0 };
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;

			if (miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
				textureDesc.ArraySize = 6;
			else
				textureDesc.ArraySize = 1;

			textureDesc.Format = format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = usage;
			textureDesc.CPUAccessFlags = cpuAccess;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.MiscFlags = miscFlag;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, pInitialData, ppTexture2D));
		}

		void CreateSRV(DXGI_FORMAT format, D3D11_SRV_DIMENSION srvDimension, ID3D11Resource* pResource, ID3D11ShaderResourceView** ppSRView)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
			resourceView.Format = format;
			resourceView.ViewDimension = srvDimension;
			resourceView.Texture2D.MostDetailedMip = 0;
			resourceView.Texture2D.MipLevels = 1;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(pResource, &resourceView, ppSRView));
		}

		Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(TextureFilter filter, TextureWrap wrap)
		{
			switch (filter)
			{
			case TextureFilter::Linear:
			{
				if (wrap == TextureWrap::Repeat)
					return DX11RenderStates::SSLinearWrap;

				if (wrap == TextureWrap::Clamp)
					return DX11RenderStates::SSLinearClamp;
			}
			}
			HNB_CORE_ASSERT(false, "Unknown SamplerState type!");
			return nullptr;
		}

		void MapData(ID3D11Resource* pResource, const Buffer& data, ImageFormat fromat)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			DX_CHECK_RESULT(DX11Context::GetDeviceContext()->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

			if (fromat == ImageFormat::RGB8)
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

			DX11Context::GetDeviceContext()->Unmap(pResource, 0);
		}

		bool CompareTexture(const Texture& tex1, const Texture& tex2)
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv1 = static_cast<ID3D11ShaderResourceView*>(tex1.GetRendererID());
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv2 = static_cast<ID3D11ShaderResourceView*>(tex2.GetRendererID());
			ID3D11Resource* resource1;
			ID3D11Resource* resource2;
			srv1->GetResource(&resource1);
			srv2->GetResource(&resource2);
			return resource1 == resource2;
		}
	}

	DX11Texture2D::DX11Texture2D(const TextureSpecification& specification, Buffer data) :m_Specification(specification),
		m_Width(specification.Width), m_Height(specification.Height)
	{
		m_DataFormat = Utils::ImageFormatToDXDataFormat(specification.Format);
		if (!data.Size)
		{
			Utils::CreateTexture(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, m_Width, m_Height, m_DataFormat, nullptr, m_Texture.GetAddressOf());
		}
		else
		{
			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = data.Data;
			subresourceData.SysMemPitch = m_Width * 4;  // size of one row in bytes
			Utils::CreateTexture(D3D11_USAGE_DEFAULT, 0, 0, m_Width, m_Height, m_DataFormat, &subresourceData, m_Texture.GetAddressOf());

			m_Data = Buffer::Copy(data);
		}
		Utils::CreateSRV(m_DataFormat, D3D11_SRV_DIMENSION_TEXTURE2D, m_Texture.Get(), m_TextureSRV.GetAddressOf());

		m_SamplerState = Utils::GetSamplerState(specification.SamplerFilter, specification.SamplerWrap);
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_Texture.Reset();
		m_TextureSRV.Reset();
	}

	void DX11Texture2D::SetData(Buffer data)
	{
		m_Data = Buffer::Copy(data);

		Utils::MapData(m_Texture.Get(), data, m_Specification.Format);
	}

	bool DX11Texture2D::operator==(const Texture& other) const
	{
		return Utils::CompareTexture(*this, other);
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, m_SamplerState.GetAddressOf());
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureSRV.GetAddressOf());
	}

	DX11TextureCube::DX11TextureCube(const TextureSpecification& specification, const std::array<Buffer, 6>& buffers) :m_Specification(specification),
		m_Width(specification.Width), m_Height(specification.Height)
	{
		m_DataFormat = Utils::ImageFormatToDXDataFormat(specification.Format);

		D3D11_SUBRESOURCE_DATA subresourceData[6] = {};
		for (size_t i = 0; i < 6; i++)
		{
			subresourceData[i].pSysMem = buffers[i].Data;
			subresourceData[i].SysMemPitch = m_Width * 4;  // size of one row in bytes
		}

		Utils::CreateTexture(D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, m_Width, m_Height, m_DataFormat, subresourceData, m_Texture.GetAddressOf());
		Utils::CreateSRV(m_DataFormat, D3D11_SRV_DIMENSION_TEXTURECUBE, m_Texture.Get(), m_TextureSRV.GetAddressOf());

		m_SamplerState = Utils::GetSamplerState(specification.SamplerFilter, specification.SamplerWrap);
	}

	DX11TextureCube::DX11TextureCube(const TextureSpecification& specification, Buffer data) :m_Specification(specification),
		m_Width(specification.Width), m_Height(specification.Height)
	{
		m_DataFormat = Utils::ImageFormatToDXDataFormat(specification.Format);
		//TODO: Create TextureCube with null data, and set data later
		if (!data.Size)
		{
			Utils::CreateTexture(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, D3D11_RESOURCE_MISC_TEXTURECUBE, m_Width, m_Height, m_DataFormat, nullptr, m_Texture.GetAddressOf());
		}
		else
		{
			uint32_t faceSize = data.Size / 6;
			D3D11_SUBRESOURCE_DATA subresourceData[6] = {};
			for (size_t i = 0; i < 6; i++)
			{
				subresourceData[i].pSysMem = data.Data + i * faceSize;
				subresourceData[i].SysMemPitch = m_Width * 4;  // size of one row in bytes
			}

			Utils::CreateTexture(D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, m_Width, m_Height, m_DataFormat, subresourceData, m_Texture.GetAddressOf());
		}
		Utils::CreateSRV(m_DataFormat, D3D11_SRV_DIMENSION_TEXTURECUBE, m_Texture.Get(), m_TextureSRV.GetAddressOf());

		m_SamplerState = Utils::GetSamplerState(specification.SamplerFilter, specification.SamplerWrap);
	}

	DX11TextureCube::~DX11TextureCube()
	{
		m_Texture.Reset();
		m_TextureSRV.Reset();
	}

	void DX11TextureCube::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, m_SamplerState.GetAddressOf());
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureSRV.GetAddressOf());
	}

	void DX11TextureCube::SetData(Buffer data)
	{
		Utils::MapData(m_Texture.Get(), data, m_Specification.Format);
	}

	bool DX11TextureCube::operator==(const Texture& other) const
	{
		return Utils::CompareTexture(*this, other);
	}
}
#endif