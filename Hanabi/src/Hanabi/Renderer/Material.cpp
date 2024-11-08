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
		BindTextures();
	}

	void Material::BindTextures() const
	{
		const auto& declarations = m_Shader->GetReflectionData();

		for (auto& reflection : declarations)
		{
			auto& it = m_Textures.find(reflection.Name);
			if (it != m_Textures.end())
			{
				it->second->Bind(reflection);
			}
		}
	}
}