#pragma once
#include "Framebuffer.h"
#include "Pipeline.h"

namespace Hanabi
{
	struct RenderPassSpecification
	{
		Ref<Pipeline> Pipeline;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& spec) :m_Specification(spec) {}
		~RenderPass() = default;

		RenderPassSpecification& GetSpecification() { return m_Specification; }
		const RenderPassSpecification& GetSpecification() const { return m_Specification; }

		Ref<Pipeline> GetPipeline() const { return m_Specification.Pipeline; }
		Ref<Framebuffer> GetTargetFramebuffer() const { return m_Specification.Pipeline->GetSpecification().TargetFramebuffer; }

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	private:
		RenderPassSpecification m_Specification;
	};
}