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

	DX11Texture2D::DX11Texture2D(const TextureSpecification& specification, Buffer data) :m_Specification(specification)
	{
		ImageSpecification imageSpec = {};
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Usage = ImageUsage::Texture2D;

		m_Image = CreateRef<DX11Image2D>(imageSpec, data);
		m_Image->Invalidate();
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
		ImageSpecification imageSpec = {};
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Usage = ImageUsage::TextureCube;

		m_Image = CreateRef<DX11Image2D>(imageSpec, data);
		m_Image->Invalidate();
	}

	DX11TextureCube::~DX11TextureCube()
	{
		m_Image->Release();
	}

	void DX11TextureCube::Bind(uint32_t slot) const
	{
		m_Image->Bind(slot);
	}

	bool DX11TextureCube::operator==(const Texture& other) const
	{
		return Utils::CompareTexture(*this, other);
	}
}
#endif