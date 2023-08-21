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

		for (size_t i = 0; i < MAX_TEXTURES; i++)
		{
			material->m_Textures[i] = other->m_Textures[i];
		}
		return material;
	}

	Material::Material(const Ref<Shader>& shader)
	{
		m_Shader = shader;
	}

	void Material::SetTextureInternal(const Ref<Texture>& texture, uint32_t index)
	{
		m_Textures[index] = texture;
	}

	Ref<Texture> Material::GetTextureInternal(uint32_t index)
	{
		HNB_CORE_ASSERT(index < MAX_TEXTURES, "Material::GetTextureInternal: index out of range!");
		return m_Textures[index];
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		BindTextures();
	}

	void Material::BindTextures() const
	{
		for (size_t i = 0; i < MAX_TEXTURES; i++)
		{
			if (m_Textures[i])
				m_Textures[i]->Bind(i);
			else
				Renderer::GetTexture<Texture2D>("White")->Bind(i);
		}
	}
}