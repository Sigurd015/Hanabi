#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	MaterialAsset::MaterialAsset() :m_Material(CreateRef<Material>(Renderer::GetDefaultShader()))
	{}

	MaterialAsset::MaterialAsset(Ref<Material> material) :m_Material(Material::Copy(material))
	{}

	Ref<Texture2D> MaterialAsset::GetDiffuse()
	{
		return m_Material->GetTexture<Texture2D>(static_cast<uint32_t>(TextureType::Diffuse));
	}

	void MaterialAsset::SetDiffuse(AssetHandle handle)
	{
		m_DiffuseTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Diffuse));
	}

	Ref<Texture2D> MaterialAsset::GetSpecular()
	{
		return m_Material->GetTexture<Texture2D>(static_cast<uint32_t>(TextureType::Specular));
	}

	void MaterialAsset::SetSpecular(AssetHandle handle)
	{
		m_SpecularTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Specular));
	}

	Ref<Texture2D> MaterialAsset::GetNormal()
	{
		return m_Material->GetTexture<Texture2D>(static_cast<uint32_t>(TextureType::Normal));
	}

	void MaterialAsset::SetNormal(AssetHandle handle)
	{
		m_NormalTexture = handle;
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		SetTexture(texture, static_cast<uint32_t>(TextureType::Normal));
	}

	void MaterialAsset::SetTexture(Ref<Texture2D> texture, uint32_t index)
	{
		m_Material->SetTexture<Texture2D>(texture, index);
	}
}