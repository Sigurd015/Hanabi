#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Engine/Asset/AssetManager.h"

namespace Hanabi
{
	MaterialAsset::MaterialAsset()
	{
		m_Material = Material::Copy(Renderer::GetDefaultMaterial());
	}

	MaterialAsset::MaterialAsset(Ref<Material> material)
	{
		m_Material = Material::Copy(material);
	}

	Ref<Texture2D> MaterialAsset::GetDiffuse()
	{
		return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Diffuse));
	}

	void MaterialAsset::SetDiffuse(AssetHandle handle)
	{
		if (m_DiffuseTexture == handle)
			return;

		m_DiffuseTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Diffuse));
	}

	void MaterialAsset::ClearDiffuse()
	{
		m_DiffuseTexture = 0;
		ClearTexture(static_cast<uint32_t>(TextureType::Diffuse));
	}

	Ref<Texture2D> MaterialAsset::GetSpecular()
	{
		return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Specular));
	}

	void MaterialAsset::SetSpecular(AssetHandle handle)
	{
		if (m_SpecularTexture == handle)
			return;

		m_SpecularTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Specular));
	}

	void MaterialAsset::ClearSpecular()
	{
		m_SpecularTexture = 0;
		ClearTexture(static_cast<uint32_t>(TextureType::Specular));
	}

	Ref<Texture2D> MaterialAsset::GetNormal()
	{
		return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Normal));
	}

	void MaterialAsset::SetNormal(AssetHandle handle)
	{
		if (m_NormalTexture == handle)
			return;

		m_NormalTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Normal));
	}

	void MaterialAsset::ClearNormal()
	{
		m_NormalTexture = 0;
		ClearTexture(static_cast<uint32_t>(TextureType::Normal));
	}

	void MaterialAsset::SetTexture(Ref<Texture2D> texture, uint32_t index)
	{
		m_Material->SetTexture(texture, index);
	}

	void MaterialAsset::ClearTexture(uint32_t index)
	{
		m_Material->SetTexture(Renderer::GetTexture("White"), index);
	}
}