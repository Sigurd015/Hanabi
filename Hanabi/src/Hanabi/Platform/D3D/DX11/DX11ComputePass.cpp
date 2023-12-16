#include "hnbpch.h"
#include "DX11ComputePass.h"
#include "DX11Context.h"
#include "DX11RenderStates.h"

namespace Hanabi
{
	DX11ComputePass::DX11ComputePass(const ComputePassSpecification& spec) : m_Specification(spec)
	{}

	void DX11ComputePass::SetInput(std::string_view name, Ref<RendererResource> resource)
	{
		m_Inputs[name.data()] = resource;
	}

	void DX11ComputePass::BindInputs()
	{
		const auto& declarations = m_Specification.Pipeline->GetSpecification().Shader->GetReflectionData();

		for (auto& reflection : declarations)
		{
			if (reflection.ResourceType == RendererResourceType::Sampler)
			{
				DX11Context::GetDeviceContext()->CSSetSamplers(reflection.Slot, 1, DX11RenderStates::SamplerStates[reflection.Name].GetAddressOf());
			}
			else
			{
				auto& it = m_Inputs.find(reflection.Name);
				if (it != m_Inputs.end())
				{
					it->second->Bind(reflection);
				}
				else
				{
					HNB_CORE_WARN("ComputePass: Missing input: {0}", reflection.Name);
				}
			}
		}
	}

	void DX11ComputePass::UnbindInputs()
	{
		static ID3D11UnorderedAccessView* s_NullUAV[] = { nullptr };
		const auto& declarations = m_Specification.Pipeline->GetSpecification().Shader->GetReflectionData();

		for (auto& reflection : declarations)
		{
			if (reflection.ResourceType == RendererResourceType::UnorderedAccess)
			{
				DX11Context::GetDeviceContext()->CSSetUnorderedAccessViews(reflection.Slot, 1, s_NullUAV, nullptr);
			}
		}
	}

	void DX11ComputePass::Dispatch(uint32_t x, uint32_t y, uint32_t z)
	{
		DX11Context::GetDeviceContext()->Dispatch(x, y, z);
	}
}