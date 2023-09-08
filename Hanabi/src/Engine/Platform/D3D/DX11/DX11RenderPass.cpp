#include "hnbpch.h"
#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11RenderPass.h"
#include "DX11Context.h"
#include "DX11RenderStates.h"

namespace Hanabi
{
	DX11RenderPass::DX11RenderPass(const RenderPassSpecification& spec) : m_Specification(spec)
	{}

	void DX11RenderPass::SetInput(std::string_view name, Ref<RendererResource> resource)
	{
		m_Inputs[name.data()] = resource;
	}

	void DX11RenderPass::BindInputs()
	{
		const ShaderReflectionData& reflectionData = m_Specification.Pipeline->GetSpecification().Shader->GetReflectionData();

		for (auto& input : m_Inputs)
		{
			const std::string& name = input.first;
			Ref<RendererResource> bindable = input.second;

			auto it = reflectionData.find(name);
			if (it != reflectionData.end())
			{
				uint32_t slot = it->second;
				bindable->Bind(slot);
			}
			else
			{
				HNB_CORE_WARN("RendererResource '{}' not found in shader!", name);
			}
		}

		// Bind Common States
		{
			auto it = reflectionData.find("u_SSLinearWrap");
			if (it != reflectionData.end())
			{
				uint32_t slot = it->second;
				DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, DX11RenderStates::SSLinearWrap.GetAddressOf());
			}
		}
		{
			auto it = reflectionData.find("u_SSLinearClamp");
			if (it != reflectionData.end())
			{
				uint32_t slot = it->second;
				DX11Context::GetDeviceContext()->PSSetSamplers(slot, 1, DX11RenderStates::SSLinearClamp.GetAddressOf());
			}
		}
	}

	Ref<Image2D> DX11RenderPass::GetOutput(uint32_t index)
	{
		return m_Specification.Pipeline->GetSpecification().TargetFramebuffer->GetImage(index);
	}

	Ref<Image2D> DX11RenderPass::GetDepthOutput()
	{
		return  m_Specification.Pipeline->GetSpecification().TargetFramebuffer->GetDepthImage();
	}
}
#endif