#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Texture.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(const TextureSpecification& specification, Buffer data = Buffer());
		~DX11Texture2D();
		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		void* GetRendererID() const override { return m_TextureView.Get(); }
		void Bind(uint32_t slot = 0) const override;
		void SetData(Buffer data) override;
		bool operator==(const Texture& other) const override;
	private:
		TextureSpecification m_Specification;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
		uint32_t m_Width, m_Height;
		DXGI_FORMAT m_DataFormat;
	};
}
#endif