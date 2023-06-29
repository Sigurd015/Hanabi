#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"

namespace Hanabi
{
	// Phong Lighting Model for now
	enum class TextureType
	{
		Diffuse = 0,
		Specular = 1,
	};

	MaterialAsset::MaterialAsset()
	{
		m_Material = Renderer::GetDefaultMaterial();
	}

	MaterialAsset::MaterialAsset(Ref<Material> material) : m_Material(material)
	{}

	Ref<Texture2D> MaterialAsset::GetDiffuse()
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Diffuse);
		return m_Material->GetTexture(index);
	}

	void MaterialAsset::SetDiffuse(Ref<Texture2D> texture)
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Diffuse);
		m_Material->SetTexture(texture, index);
	}

	void MaterialAsset::ClearDiffuse()
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Diffuse);
		m_Material->SetTexture(nullptr, index);
	}

	Ref<Texture2D> MaterialAsset::GetSpecular()
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Specular);
		return m_Material->GetTexture(index);
	}

	void MaterialAsset::SetSpecular(Ref<Texture2D> texture)
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Specular);
		m_Material->SetTexture(texture, index);
	}

	void MaterialAsset::ClearSpecular()
	{
		uint32_t index = static_cast<uint32_t>(TextureType::Specular);
		m_Material->SetTexture(nullptr, index);
	}
}