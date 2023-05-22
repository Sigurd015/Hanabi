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
		DX11Texture2D(const std::string& path);
		DX11Texture2D(uint32_t width, uint32_t height);
		~DX11Texture2D();
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		void* GetRendererID() const override { return m_TextureView.Get(); }
		virtual bool IsLoaded() const override { return m_IsLoaded; }
		void Bind(uint32_t slot = 0) const override;
		void SetData(void* data, uint32_t size) override;
		virtual const std::string& GetPath() const override { return m_Path; }
		bool operator==(const Texture& other) const override;
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		DXGI_FORMAT m_DataFormat;
	};
}
#endif