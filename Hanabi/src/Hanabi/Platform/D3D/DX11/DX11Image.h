#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Hanabi/Renderer/Image.h"
#include "Hanabi/Platform/D3D/DXCommon.h"
#include "DX11Context.h"

#include <d3d11.h>

namespace Hanabi
{
	namespace Utils
	{
		// UNORM: Unsigned normalized
		// SNORM: Signed normalized
		// FLOAT: Signed float
		inline DXGI_FORMAT ImageFormatToDXTextureFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::SRGB:            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case ImageFormat::RGBA:            return DXGI_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA16F:         return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case ImageFormat::RGBA32F:         return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24G8_TYPELESS;
			case ImageFormat::DEPTH32F:        return DXGI_FORMAT_R32_TYPELESS;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

		inline DXGI_FORMAT ImageFormatToDXDepthSRVFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_R32_FLOAT;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

		inline DXGI_FORMAT ImageFormatToDXDepthDSVFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case ImageFormat::DEPTH32F: return DXGI_FORMAT_D32_FLOAT;
			}

			HNB_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

		inline DXGI_SAMPLE_DESC Multisample(UINT count, DXGI_FORMAT format)
		{
			DXGI_SAMPLE_DESC sampleDesc{};
			sampleDesc.Count = count;
			if (count > 1)
			{
				UINT  m_Quality;
				DX11Context::GetDevice()->CheckMultisampleQualityLevels(format, count, &m_Quality);
				sampleDesc.Quality = m_Quality - 1;
			}
			else
			{
				sampleDesc.Quality = 0;
			}
			return sampleDesc;
		}
	}

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

		// D3D Interop API
		DXGI_FORMAT GetDXGIFormat() const { return m_DataFormat; }
		ComPtr<ID3D11Texture2D> GetTexture() const { return m_Texture; }
		ComPtr<ID3D11ShaderResourceView> GetTextureSRV() const { return m_TextureSRV; }
	private:
		ImageSpecification m_Specification;
		Buffer m_ImageData;
		DXGI_FORMAT m_DataFormat;
		ComPtr<ID3D11Texture2D> m_Texture;
		ComPtr<ID3D11ShaderResourceView> m_TextureSRV;
	};
}
#endif