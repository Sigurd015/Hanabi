#pragma once
#include "Framebuffer.h"

namespace Hanabi
{
	struct RenderPassSpecification
	{
		Ref<Framebuffer> TargetFramebuffer;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& spec) :m_Specification(spec) {}
		~RenderPass() = default;

		RenderPassSpecification& GetSpecification() { return m_Specification; }
		const RenderPassSpecification& GetSpecification() const { return m_Specification; }

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	private:
		RenderPassSpecification m_Specification;
	};
}