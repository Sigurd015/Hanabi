#include "hnbpch.h"
#include "Material.h"

namespace Hanabi
{
	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return CreateRef<Material>(shader);
	}

	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{}

	Material::~Material()
	{}

	void Material::SetTexture(const Ref<Texture>& texture, uint32_t index)
	{
		m_Textures[index] = texture;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		BindTextures();
	}

	void Material::BindTextures() const
	{
		for (auto& texture : m_Textures)
		{
			texture.second->Bind(texture.first);
		}
	}
}