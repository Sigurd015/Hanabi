#pragma once
#include "Engine/Core/Base.h"

namespace Hanabi
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format, const glm::vec4& clearColor = { 0.3f,0.3f,0.3f,1.0f })
			: TextureFormat(format), ClearColor(clearColor)
		{}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		glm::vec4 ClearColor;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};
	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		float DepthClearValue = 1.0f;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual void ClearAndBind() = 0;
		virtual void Unbind() = 0;
		virtual void* GetColorAttachment(uint32_t index = 0) const = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
		
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}