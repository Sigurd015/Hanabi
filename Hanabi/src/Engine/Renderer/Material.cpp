#include "hnbpch.h"
#include "Material.h"
#include "Renderer.h"

namespace Hanabi
{
	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return CreateRef<Material>(shader);
	}

	Ref<Material> Material::Copy(const Ref<Material>& other)
	{
		Ref<Material> material = CreateRef<Material>(other->GetShader());

		for (auto& texture : other->m_Textures)
		{
			material->m_Textures[texture.first] = texture.second;
		}

		return material;
	}

	Material::Material(const Ref<Shader>& shader)
	{
		m_Shader = shader;
	}

	void Material::SetTextureInternal(const std::string& name, const Ref<Texture>& texture)
	{
		m_Textures[name] = texture;
	}

	Ref<Texture> Material::GetTextureInternal(const std::string& name)
	{
		HNB_CORE_ASSERT(m_Textures.find(name) != m_Textures.end(), "Material::GetTextureInternal: Texture not found");
		return m_Textures[name];
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		BindTextures();
	}

	void Material::BindTextures() const
	{
		const ShaderReflectionData& reflectionData = m_Shader->GetReflectionData();

		for (auto& texture : m_Textures)
		{
			auto it = reflectionData.find(texture.first);
			if (it != reflectionData.end())
			{
				uint32_t slot = it->second;
				if (texture.second)
					texture.second->Bind(slot);
				else
					Renderer::GetTexture<Texture2D>("White")->Bind(slot);
			}
			else
			{
				HNB_CORE_WARN("Texture '{}' not found in shader!", texture.first);
			}
		}
	}
}