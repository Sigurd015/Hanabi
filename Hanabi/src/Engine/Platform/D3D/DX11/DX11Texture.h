#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Texture.h"
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
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }
		void* GetRendererID() const override { return m_Image->GetRendererID(); }
		void Bind(uint32_t slot = 0) const override;
		virtual Ref<Image2D> GetImage() const override { return m_Image; }
		virtual Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		bool operator==(const Texture& other) const override;
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
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }
		void* GetRendererID() const override { return m_Image->GetRendererID(); }
		void Bind(uint32_t slot = 0) const override;
		bool operator==(const Texture& other) const override;
	private:
		Ref<DX11Image2D> m_Image;
		TextureSpecification m_Specification;
	};
}
#endif