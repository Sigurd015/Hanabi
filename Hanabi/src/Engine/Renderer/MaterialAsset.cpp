#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	MaterialAsset::MaterialAsset() :m_Material(CreateRef<Material>(Renderer::GetDefaultShader()))
	{
		m_Material->SetTexture<Texture2D>("u_Diffuse", Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>("u_Specular", Renderer::GetTexture<Texture2D>("Black"));
		m_Material->SetTexture<Texture2D>("u_Normal", Renderer::GetTexture<Texture2D>("White"));
	}

	MaterialAsset::MaterialAsset(Ref<Material> material) :m_Material(Material::Copy(material))
	{}

	Ref<Texture2D> MaterialAsset::GetDiffuse()
	{
		return m_Material->GetTexture<Texture2D>("u_Diffuse");
	}

	void MaterialAsset::SetDiffuse(AssetHandle handle)
	{
		m_DiffuseTexture = handle;
		Ref<Texture2D>& texture = Renderer::GetTexture<Texture2D>("White");
		if (AssetManager::IsAssetHandleValid(m_DiffuseTexture))
		{
			texture = AssetManager::GetAsset<Texture2D>(m_DiffuseTexture);
		}
		SetTexture("u_Diffuse", texture);
	}

	Ref<Texture2D> MaterialAsset::GetSpecular()
	{
		return m_Material->GetTexture<Texture2D>("u_Specular");
	}

	void MaterialAsset::SetSpecular(AssetHandle handle)
	{
		m_SpecularTexture = handle;
		Ref<Texture2D>& texture = Renderer::GetTexture<Texture2D>("Black");
		if (AssetManager::IsAssetHandleValid(m_SpecularTexture))
		{
			texture = AssetManager::GetAsset<Texture2D>(m_SpecularTexture);
		}
		SetTexture("u_Specular", texture);
	}

	Ref<Texture2D> MaterialAsset::GetNormal()
	{
		return m_Material->GetTexture<Texture2D>("u_Normal");
	}

	void MaterialAsset::SetNormal(AssetHandle handle)
	{
		m_NormalTexture = handle;
		Ref<Texture2D>& texture = Renderer::GetTexture<Texture2D>("White");
		if (AssetManager::IsAssetHandleValid(m_NormalTexture))
		{
			texture = AssetManager::GetAsset<Texture2D>(m_NormalTexture);
		}
		SetTexture("u_Normal", texture);
	}

	void MaterialAsset::SetTexture(const std::string& name, const Ref<Texture2D>& texture)
	{
		m_Material->SetTexture<Texture2D>(name, texture);
	}
}