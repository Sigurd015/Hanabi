#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Image.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11Image2D : public Image2D
	{
	public:
		DX11Image2D(const ImageSpecification& specification, Buffer buffer= Buffer());
		~DX11Image2D();

		virtual void Resize(const glm::uvec2& size) override { Resize(size.x, size.y); }
		virtual void Resize(const uint32_t width, const uint32_t height) override;
		virtual void Invalidate() override;
		virtual void Release() override;
		virtual void Bind(uint32_t slot) const override;
		virtual void* GetRendererID() const override { return m_TextureSRV.Get(); }

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual glm::uvec2 GetSize() const override { return { m_Specification.Width, m_Specification.Height }; }

		virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }

		virtual ImageSpecification& GetSpecification() override { return m_Specification; }
		virtual const ImageSpecification& GetSpecification() const override { return m_Specification; }

		virtual Buffer GetBuffer() const override { return m_ImageData; }
		virtual Buffer& GetBuffer() override { return m_ImageData; }

		void SetData(Buffer data);

		DXGI_FORMAT GetFormat() const { return m_DataFormat; }
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const { return m_Texture; }
	private:
		ImageSpecification m_Specification;
		Buffer m_ImageData;
		DXGI_FORMAT m_DataFormat;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureSRV;
	};
}
#endif