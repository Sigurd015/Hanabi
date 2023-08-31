#include "hnbpch.h"
#include "DX11RenderPass.h"

namespace Hanabi
{
	DX11RenderPass::DX11RenderPass(const RenderPassSpecification& spec) : m_Specification(spec)
	{}

	void DX11RenderPass::SetInput(std::string_view name, Ref<ConstantBuffer> constantBuffer)
	{
		m_Inputs[name.data()] = constantBuffer;
	}

	void DX11RenderPass::SetInput(std::string_view name, Ref<TextureCube> textureCube)
	{
		m_Inputs[name.data()] = textureCube;
	}

	void DX11RenderPass::SetInput(std::string_view name, Ref<Texture2D> texture)
	{
		m_Inputs[name.data()] = texture;
	}

	void DX11RenderPass::BindInputs()
	{
		const ShaderReflectionData& reflectionData = m_Specification.Pipeline->GetSpecification().Shader->GetReflectionData();

		for (auto& input : m_Inputs)
		{
			const std::string& name = input.first;
			Ref<RendererResource> bindable = input.second;

			if (bindable->GetRendererResourceType() == RendererResourceType::ConstantBuffer)
			{
				Ref<ConstantBuffer> constantBuffer = bindable->GetAs<ConstantBuffer>();

				auto it = reflectionData.find(name);
				if (it != reflectionData.end())
				{
					uint32_t slot = it->second;
					constantBuffer->Bind(slot);
				}
				else
				{
					HNB_CORE_WARN("Constant buffer '{}' not found in shader!", name);
				}
			}
			else if (bindable->GetRendererResourceType() == RendererResourceType::Texture2D)
			{
				Ref<Texture2D> texture = bindable->GetAs<Texture2D>();

				auto it = reflectionData.find(name);
				if (it != reflectionData.end())
				{
					uint32_t slot = it->second;
					texture->Bind(slot);
				}
				else
				{
					HNB_CORE_WARN("Texture2D '{}' not found in shader!", name);
				}
			}
			else if (bindable->GetRendererResourceType() == RendererResourceType::TextureCube)
			{
				Ref<TextureCube> textureCube = bindable->GetAs<TextureCube>();

				auto it = reflectionData.find(name);
				if (it != reflectionData.end())
				{
					uint32_t slot = it->second;
					textureCube->Bind(slot);
				}
				else
				{
					HNB_CORE_WARN("TextureCube '{}' not found in shader!", name);
				}
			}
		}
	}

	Ref<Texture2D> DX11RenderPass::GetOutput(uint32_t index)
	{
		return Ref<Texture2D>();
	}

	Ref<Texture2D> DX11RenderPass::GetDepthOutput()
	{
		return Ref<Texture2D>();
	}
}

