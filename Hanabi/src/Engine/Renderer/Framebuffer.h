#pragma once
#include "Engine/Core/Base.h"
#include "Image.h"

namespace Hanabi
{
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format) :TextureFormat(format) {}

		ImageFormat TextureFormat;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments)
		{}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		float DepthClearValue = 1.0f;

		// TODO: Make a better way to do mouse picking
		int MousePickClearValue = -1;

		bool SwapChainTarget = false;

		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
	};

	class Framebuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void ClearAttachment(const glm::vec4& color) = 0;
		virtual void* GetColorAttachment(uint32_t attachmentIndex = 0) const = 0;
		virtual void* GetDepthAttachment() const = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}