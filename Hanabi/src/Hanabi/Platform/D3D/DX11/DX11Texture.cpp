#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11Texture.h"
#include "DX11Context.h"
#include "Hanabi/Core/UUID.h"
#include "Hanabi/Platform/D3D/DXCommon.h"
#include "DX11RenderStates.h"

#include <stb_image.h>

namespace Hanabi
{
	namespace Utils
	{
		bool CompareTexture(const Texture& tex1, const Texture& tex2)
		{
			ComPtr<ID3D11ShaderResourceView> srv1 = static_cast<ID3D11ShaderResourceView*>(tex1.GetRendererID());
			ComPtr<ID3D11ShaderResourceView> srv2 = static_cast<ID3D11ShaderResourceView*>(tex2.GetRendererID());
			ID3D11Resource* resource1;
			ID3D11Resource* resource2;
			srv1->GetResource(&resource1);
			srv2->GetResource(&resource2);
			return resource1 == resource2;
		}
	}

	DX11Texture2D::DX11Texture2D(const TextureSpecification& specification, Buffer data) :m_Specification(specification)
	{
		ImageSpecification imageSpec = {};
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Usage = ImageUsage::Texture2D;
		imageSpec.Mips = specification.GenerateMips ? 0 : 1;

		HNB_CORE_ASSERT(data, "DX11Texture2D: Data is null!");

		m_Image = CreateRef<DX11Image2D>(imageSpec, data);
		m_Image->Invalidate();

		if (specification.GenerateMips)
		{
			DX11Context::GetDeviceContext()->GenerateMips(m_Image->GetTextureSRV().Get());
		}
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_Image->Release();
	}

	bool DX11Texture2D::operator==(const Texture& other) const
	{
		return Utils::CompareTexture(*this, other);
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		m_Image->Bind(slot);
	}

	DX11TextureCube::DX11TextureCube(const TextureSpecification& specification, Buffer data) :m_Specification(specification)
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = m_Specification.Width;
		textureDesc.Height = m_Specification.Height;
		textureDesc.ArraySize = 6;
		textureDesc.Format = Utils::ImageFormatToDXTextureFormat(m_Specification.Format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.CPUAccessFlags = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceView = {};
		resourceView.Format = textureDesc.Format;
		resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		resourceView.Texture2D.MostDetailedMip = 0;

		if (m_Specification.GenerateMips)
		{
			textureDesc.MipLevels = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			resourceView.Texture2D.MipLevels = -1;
		}
		else
		{
			textureDesc.MipLevels = 1;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			resourceView.Texture2D.MipLevels = 1;
		}

		if (data)
		{
			m_ImageData = Buffer::Copy(data);

			uint32_t faceSize = m_ImageData.Size / 6;
			D3D11_SUBRESOURCE_DATA subresourceData[6] = {};
			for (size_t i = 0; i < 6; i++)
			{
				subresourceData[i].pSysMem = m_ImageData.Data + i * faceSize;
				subresourceData[i].SysMemPitch = m_Specification.Width * 4;  // size of one row in bytes
			}
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, subresourceData, m_TextureCube.GetAddressOf()));

			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(m_TextureCube.Get(), &resourceView, m_TextureCubeSRV.GetAddressOf()));

			if (m_Specification.GenerateMips)
			{
				DX11Context::GetDeviceContext()->GenerateMips(m_TextureCubeSRV.Get());
			}
		}
		else
		{
			textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateTexture2D(&textureDesc, nullptr, m_TextureCube.GetAddressOf()));

			DX_CHECK_RESULT(DX11Context::GetDevice()->CreateShaderResourceView(m_TextureCube.Get(), &resourceView, m_TextureCubeSRV.GetAddressOf()));
		}
	}

	DX11TextureCube::~DX11TextureCube()
	{
		m_TextureCube.Reset();
		m_TextureCubeSRV.Reset();
		m_TextureCubeUAV.Reset();
	}

	void DX11TextureCube::Bind(uint32_t slot) const
	{
		DX11Context::GetDeviceContext()->PSSetShaderResources(slot, 1, m_TextureCubeSRV.GetAddressOf());
	}

	bool DX11TextureCube::operator==(const Texture& other) const
	{
		return Utils::CompareTexture(*this, other);
	}

	void DX11TextureCube::GenerateMips() const
	{
		DX11Context::GetDeviceContext()->GenerateMips(m_TextureCubeSRV.Get());
	}

	/// <summary>
	/// Notice: Default mipSlice is 0, which means the first mip level
	/// When call this function at 0 mipSlice, make sure call GenerateMips() after that to generate mips
	/// Call this function at other mipSlice, means manually generate mips, like use compute shader to generate mips, no need to call GenerateMips()
	/// </summary>
	void DX11TextureCube::CreateUAV(uint32_t mipSlice)
	{
		D3D11_TEXTURE2D_DESC desc;
		m_TextureCube->GetDesc(&desc);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = desc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = mipSlice;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = desc.ArraySize;

		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateUnorderedAccessView(m_TextureCube.Get(), &uavDesc, m_TextureCubeUAV.GetAddressOf()));
	}
}
#endif