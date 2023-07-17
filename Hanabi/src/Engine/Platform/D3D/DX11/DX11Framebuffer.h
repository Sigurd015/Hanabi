#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Framebuffer.h"

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11Framebuffer : public Framebuffer
	{
	public:
		DX11Framebuffer(const FramebufferSpecification& spec);
		~DX11Framebuffer();

		void Bind() override;
		void Unbind() override;
		void ClearAttachment() override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void* GetColorAttachment(uint32_t attachmentIndex = 0) const override;
		void* GetDepthAttachment() const override;

		void Resize(uint32_t width, uint32_t height) override;

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();

		FramebufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = ImageFormat::None;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_ColorAttachmentRTV;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_ColorAttachmentTextures;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_ColorAttachmentSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilAttachment;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilAttachmentsTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_DepthStencilSRV;
	};
}
#endif