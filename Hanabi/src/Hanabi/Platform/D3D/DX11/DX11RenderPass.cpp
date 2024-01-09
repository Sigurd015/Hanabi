#include "hnbpch.h"
#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11RenderPass.h"
#include "DX11Context.h"
#include "DX11RenderStates.h"
#include "DX11Shader.h"

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
		const auto& declarations = m_Specification.Pipeline->GetSpecification().Shader->GetReflectionData();

		for (auto& reflection : declarations)
		{
			if (reflection.ResourceType == RendererResourceType::Sampler)
			{
				switch (reflection.Stage)
				{
				case ShaderType::VertexShader:
					DX11Context::GetDeviceContext()->VSSetSamplers(reflection.Slot, 1, DX11RenderStates::SamplerStates[reflection.Name].GetAddressOf());
					break;
				case ShaderType::PixelShader:
					DX11Context::GetDeviceContext()->PSSetSamplers(reflection.Slot, 1, DX11RenderStates::SamplerStates[reflection.Name].GetAddressOf());
					break;
				case ShaderType::GeometryShader:
					DX11Context::GetDeviceContext()->GSSetSamplers(reflection.Slot, 1, DX11RenderStates::SamplerStates[reflection.Name].GetAddressOf());
					break;
				}
			}
			else
			{
				auto& it = m_Inputs.find(reflection.Name);
				if (it != m_Inputs.end())
				{
					it->second->Bind(reflection);
				}
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