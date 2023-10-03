#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	MaterialAsset::MaterialAsset() :m_Material(CreateRef<Material>(Renderer::GetDefaultShader()))
	{
		m_Material->SetTexture<Texture2D>("u_AlbedoTex", Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>("u_MetalnessTex", Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>("u_RoughnessTex", Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>("u_NormalTex", Renderer::GetTexture<Texture2D>("Black"));
	}

	MaterialAsset::MaterialAsset(Ref<Material> material) :m_Material(Material::Copy(material))
	{}

	Ref<Texture2D> MaterialAsset::GetAlbedoTex()
	{
		return m_Material->GetTexture<Texture2D>("u_AlbedoTex");
	}

	Ref<Texture2D> MaterialAsset::GetNormalTex()
	{
		return m_Material->GetTexture<Texture2D>("u_NormalTex");
	}

	Ref<Texture2D> MaterialAsset::GetMetalnessTex()
	{
		return m_Material->GetTexture<Texture2D>("u_MetalnessTex");
	}

	Ref<Texture2D> MaterialAsset::GetRoughnessTex()
	{
		return m_Material->GetTexture<Texture2D>("u_RoughnessTex");
	}

	void MaterialAsset::SetAlbedoTex(AssetHandle handle)
	{
		m_AlbedoTexture = handle;
		SetTextureInternal("u_AlbedoTex", m_AlbedoTexture);
	}

	void MaterialAsset::SetMetalnessTex(AssetHandle handle)
	{
		m_MetalnessTexture = handle;
		SetTextureInternal("u_MetalnessTex", m_MetalnessTexture);
	}

	void MaterialAsset::SetRoughnessTex(AssetHandle handle)
	{
		m_RoughnessTexture = handle;
		SetTextureInternal("u_RoughnessTex", m_RoughnessTexture);
	}

	void MaterialAsset::SetNormalTex(AssetHandle handle)
	{
		m_NormalTexture = handle;
		SetTextureInternal("u_NormalTex", m_NormalTexture);
	}

	void MaterialAsset::SetTextureInternal(const std::string& name, AssetHandle handle, const std::string& defaultTexName)
	{
		Ref<Texture2D>& texture = Renderer::GetTexture<Texture2D>(defaultTexName);
		if (AssetManager::IsAssetHandleValid(handle))
		{
			texture = AssetManager::GetAsset<Texture2D>(handle);
		}
		m_Material->SetTexture<Texture2D>(name, texture);
	}
}