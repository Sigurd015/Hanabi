#pragma once
#include "Engine/Renderer/RenderPass.h"

namespace Hanabi
{
	class DX11RenderPass : public RenderPass
	{
	public:
		DX11RenderPass(const RenderPassSpecification& spec);
		virtual ~DX11RenderPass() = default;

		virtual void SetInput(std::string_view name, Ref<ConstantBuffer> constantBuffer) override;
		virtual void SetInput(std::string_view name, Ref<TextureCube> textureCube) override;
		virtual void SetInput(std::string_view name, Ref<Texture2D> texture) override;

		virtual void BindInputs() override;

		virtual Ref<Texture2D> GetOutput(uint32_t index) override;
		virtual Ref<Texture2D> GetDepthOutput() override;

		virtual RenderPassSpecification& GetSpecification() override { return m_Specification; }
		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

		virtual Ref<Pipeline> GetPipeline() const override { return m_Specification.Pipeline; }
		virtual Ref<Framebuffer> GetTargetFramebuffer() const override { return m_Specification.Pipeline->GetSpecification().TargetFramebuffer; }

	private:
		RenderPassSpecification m_Specification;
		std::unordered_map<std::string, Ref<RendererResource>> m_Inputs;
	};
}