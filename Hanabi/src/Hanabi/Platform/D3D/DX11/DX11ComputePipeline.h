#pragma once
#include "Hanabi/Renderer/ComputePipeline.h"

namespace Hanabi
{
	class DX11ComputePipeline : public ComputePipeline
	{
	public:
		DX11ComputePipeline(const ComputePipelineSpecification& spec);
		virtual ~DX11ComputePipeline() = default;

		virtual void Bind() override;

		virtual ComputePipelineSpecification& GetSpecification() override { return m_Specification; }
		virtual const ComputePipelineSpecification& GetSpecification() const override { return m_Specification; }
	private:
		ComputePipelineSpecification m_Specification;
	};
}