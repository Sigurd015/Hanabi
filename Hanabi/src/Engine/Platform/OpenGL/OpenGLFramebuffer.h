#pragma once
#pragma once
#include "Engine/Renderer/Framebuffer.h"

namespace Hanabi
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		~OpenGLFramebuffer();
		void Bind() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height) override;
		void ClearAttachment() override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void* GetColorAttachment(uint32_t index = 0) const override
		{
			HNB_CORE_ASSERT(index < m_ColorAttachments.size()); return (void*)m_ColorAttachments[index];
		}
		void* GetDepthAttachment() const override
		{
			return (void*)m_DepthAttachment;
		}

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();
		uint32_t m_RendererID;
		FramebufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}