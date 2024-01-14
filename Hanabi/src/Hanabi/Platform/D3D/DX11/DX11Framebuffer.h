#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/Framebuffer.h"
#include "DX11Image.h"
#include "Hanabi/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Framebuffer : public Framebuffer
	{
	public:
		DX11Framebuffer(const FramebufferSpecification& spec);
		~DX11Framebuffer();

		virtual void Bind() override;
		virtual void ClearAttachment() override;
		virtual void ClearAttachment(uint32_t attachmentIndex) override;
		virtual void ClearDepthAttachment() override;

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual void SetClearColor(const glm::vec4& color) override { m_Specification.ClearColor = color; }

		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const override
		{
			HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "DX11Framebuffer::GetImage - Invalid attachment index!");
			return m_ColorAttachments[attachmentIndex];
		}

		virtual Ref<Image2D> GetDepthImage() const override
		{
			HNB_CORE_ASSERT(m_DSAttachment != nullptr, "DX11Framebuffer::GetDepthImage - No depth attachment!");
			return m_DSAttachment;
		}

		void Resize(uint32_t width, uint32_t height) override;

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Release();
		void Invalidate();

		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_AttachmentSpecifications;

		std::vector<Ref<DX11Image2D>> m_ColorAttachments;
		std::vector<ComPtr<ID3D11RenderTargetView>> m_ColorAttachmentRTV;

		Ref<DX11Image2D> m_DSAttachment = nullptr;
		ComPtr<ID3D11DepthStencilView> m_DSAttachmentDSV;
	};
}
#endif