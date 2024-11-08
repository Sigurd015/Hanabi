#pragma once
#include "Hanabi/Core/Base.h"
#include "Image.h"

#include <map>

namespace Hanabi
{
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format, uint32_t layers = 1) :TextureFormat(format), Layers(layers) {}

		ImageFormat TextureFormat;
		uint32_t Layers = 0;
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
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		bool SwapChainTarget = false;

		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1; // Multisampling

		std::map<uint32_t, Ref<Image2D>> ExistingImages;
	};

	class Framebuffer
	{
	public:
		virtual void Bind() = 0;

		virtual void ClearAttachment() = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex) = 0;
		virtual void ClearDepthAttachment() = 0;

		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const = 0;
		virtual Ref<Image2D> GetDepthImage() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}