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
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		void ClearAttachment(const glm::vec4& color) override;
		virtual void ClearAttachment() override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		//TODO: Implement
		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const override
		{
			HNB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size()); return nullptr;
		}

		virtual Ref<Image2D> GetDepthImage() const override { return nullptr; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();
		uint32_t m_RendererID;
		FramebufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = ImageFormat::None;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}