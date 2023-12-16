#include "hnbpch.h"
#include "DX11ComputePipeline.h"

namespace Hanabi
{
	DX11ComputePipeline::DX11ComputePipeline(const ComputePipelineSpecification& spec) : m_Specification(spec)
	{}

	void DX11ComputePipeline::Bind()
	{
		m_Specification.Shader->Bind();
	}
}