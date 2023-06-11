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

	void Material::Bind() const
	{
		m_Shader->Bind();

		BindTextures();
	}

	void Material::BindTextures() const
	{
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}
}