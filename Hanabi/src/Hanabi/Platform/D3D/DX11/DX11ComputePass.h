#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/ComputePass.h"

namespace Hanabi
{
	class DX11ComputePass : public ComputePass
	{
	public:
		DX11ComputePass(const ComputePassSpecification& spec);
		virtual ~DX11ComputePass() = default;

		virtual void SetInput(std::string_view name, Ref<RendererResource> resource) override;
		virtual void BindInputs() override;
		virtual void UnbindInputs() override;
		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) override;

		virtual Ref<ComputePipeline> GetPipeline() const override { return m_Specification.Pipeline; }
		virtual ComputePassSpecification& GetSpecification() override { return m_Specification; }
		virtual const ComputePassSpecification& GetSpecification() const override { return m_Specification; }
	private:
		ComputePassSpecification m_Specification;
		std::unordered_map<std::string, Ref<RendererResource>> m_Inputs;
	};
}
#endif