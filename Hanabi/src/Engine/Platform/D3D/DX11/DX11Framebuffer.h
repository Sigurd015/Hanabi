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
		void ClearAndBind() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		void* GetColorAttachment(uint32_t index = 0) const override;
		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();
		FramebufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_RenderTargetAttachments;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_RenderTargetAttachmentsTextures;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_ShaderResourceViews;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilAttachment;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilAttachmentsTexture;
	};
}
#endif