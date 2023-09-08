#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Framebuffer.h"
#include "DX11Image.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Framebuffer : public Framebuffer
	{
	public:
		DX11Framebuffer(const FramebufferSpecification& spec);
		~DX11Framebuffer();

		void Bind() override;
		void Unbind() override;
		void ClearAttachment(const glm::vec4& color) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const override
		{
			HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
			return m_ColorAttachments[attachmentIndex];
		}

		virtual Ref<Image2D> GetDepthImage() const override
		{
			HNB_CORE_ASSERT(m_DSAttachment != nullptr);
			return m_DSAttachment;
		}

		void Resize(uint32_t width, uint32_t height) override;

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();

		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = ImageFormat::None;

		std::vector<Ref<DX11Image2D>> m_ColorAttachments;
		std::vector<ComPtr<ID3D11RenderTargetView>> m_ColorAttachmentRTV;

		Ref<DX11Image2D> m_DSAttachment = nullptr;
		ComPtr<ID3D11DepthStencilView> m_DSAttachmentDSV;
	};
}
#endif