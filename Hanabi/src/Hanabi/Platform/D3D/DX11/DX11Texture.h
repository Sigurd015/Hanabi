#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Hanabi/Renderer/Texture.h"
#include "DX11Image.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(const TextureSpecification& specification, Buffer data = Buffer());
		~DX11Texture2D();
		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		virtual ImageFormat GetFormat() const override { return m_Specification.Format; }
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }
		void* GetRendererID() const override { return m_Image->GetRendererID(); }
		void Bind(const ShaderResourceDeclaration& declaration) const override;
		virtual Ref<Image2D> GetImage() const override { return m_Image; }
		virtual Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		bool operator==(const Texture& other) const override;

		// D3D Interop API
		ComPtr<ID3D11ShaderResourceView> GetTextureSRV() const { return m_Image->GetTextureSRV(); }
	private:
		Ref<DX11Image2D> m_Image;
		TextureSpecification m_Specification;
	};

	class DX11TextureCube : public TextureCube
	{
	public:
		DX11TextureCube(const TextureSpecification& specification, Buffer data = Buffer());
		~DX11TextureCube();
		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		virtual ImageFormat GetFormat() const override { return m_Specification.Format; }
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }
		void* GetRendererID() const override { return m_TextureCubeSRV.Get(); }
		void Bind(const ShaderResourceDeclaration& declaration) const override;
		bool operator==(const Texture& other) const override;

		// D3D Interop API
		void CreateUAV(uint32_t mipSlice = 0);
		void GenerateMips() const;
		ComPtr<ID3D11Texture2D> GetTextureCube() const { return m_TextureCube; }
		ComPtr<ID3D11UnorderedAccessView> GetUAV() const { return m_TextureCubeUAV; }
		ComPtr<ID3D11ShaderResourceView> GetTextureSRV() const { return m_TextureCubeSRV; }
	private:
		Buffer m_ImageData;
		ComPtr<ID3D11Texture2D> m_TextureCube;
		ComPtr<ID3D11ShaderResourceView> m_TextureCubeSRV;
		ComPtr<ID3D11UnorderedAccessView> m_TextureCubeUAV;
		TextureSpecification m_Specification;
	};
}
#endif