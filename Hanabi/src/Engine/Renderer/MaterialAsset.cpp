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
		return m_Material->GetTexture<Texture2D>("u_Diffuse");
	}

	void MaterialAsset::SetDiffuse(AssetHandle handle)
	{
		m_DiffuseTexture = handle;
		SetTexture("u_Diffuse", m_DiffuseTexture);
	}

	Ref<Texture2D> MaterialAsset::GetSpecular()
	{
		return m_Material->GetTexture<Texture2D>("u_Specular");
	}

	void MaterialAsset::SetSpecular(AssetHandle handle)
	{
		m_SpecularTexture = handle;
		SetTexture("u_Specular", m_SpecularTexture);
	}

	Ref<Texture2D> MaterialAsset::GetNormal()
	{
		return m_Material->GetTexture<Texture2D>("u_Normal");
	}

	void MaterialAsset::SetNormal(AssetHandle handle)
	{
		m_NormalTexture = handle;
		SetTexture("u_Normal", m_NormalTexture);
	}

	void MaterialAsset::SetTexture(const std::string& name, AssetHandle handle)
	{
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
		m_Material->SetTexture<Texture2D>(name, texture);
	}
}