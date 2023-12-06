#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Hanabi/Renderer/RenderPass.h"

namespace Hanabi
{
	class DX11RenderPass : public RenderPass
	{
	public:
		DX11RenderPass(const RenderPassSpecification& spec);
		virtual ~DX11RenderPass() = default;

		virtual void SetInput(std::string_view name, Ref<RendererResource> resource) override;

		virtual void BindInputs() override;

		virtual Ref<Image2D> GetOutput(uint32_t index = 0) override;
		virtual Ref<Image2D> GetDepthOutput() override;

		virtual RenderPassSpecification& GetSpecification() override { return m_Specification; }
		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

		virtual Ref<Pipeline> GetPipeline() const override { return m_Specification.Pipeline; }
		virtual Ref<Framebuffer> GetTargetFramebuffer() const override { return m_Specification.Pipeline->GetSpecification().TargetFramebuffer; }

	private:
		RenderPassSpecification m_Specification;
		std::unordered_map<std::string, Ref<RendererResource>> m_Inputs;
	};
}
#endif