#include "hnbpch.h"
#include "Material.h"
#include "Renderer.h"

namespace Hanabi
{
	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return CreateRef<Material>(shader);
	}

	Material::Material(const Ref<Shader>& shader)
	{
		if (shader)
			m_Shader = shader;
		else
			m_Shader = Renderer::GetShader("3DStaticMesh_Default");
	}

	Material::~Material()
	{}

	void Material::SetTexture(const Ref<Texture2D>& texture, uint32_t index)
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
			if (texture.second)
				texture.second->Bind(texture.first);
		}
	}

	Ref<Texture2D> Material::GetTexture(uint32_t index)
	{
		if (m_Textures.find(index) != m_Textures.end())
			return m_Textures[index];

		return nullptr;
	}
}