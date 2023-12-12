#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	static const std::string s_AlbedoMap = "u_AlbedoTex";
	static const std::string s_NormalMap = "u_NormalTex";
	static const std::string s_MetallicRoughnessMap = "u_MetallicRoughnessTex";

	MaterialAsset::MaterialAsset() :m_Material(CreateRef<Material>(Renderer::GetShader("DeferredGeometry")))
	{
		m_Material->SetTexture<Texture2D>(s_AlbedoMap, Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>(s_MetallicRoughnessMap, Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>(s_NormalMap, Renderer::GetTexture<Texture2D>("White"));
	}

	MaterialAsset::MaterialAsset(Ref<Material> material) :m_Material(Material::Copy(material))
	{}

	Ref<Texture2D> MaterialAsset::GetAlbedoTex()
	{
		return m_Material->GetTexture<Texture2D>(s_AlbedoMap);
	}

	Ref<Texture2D> MaterialAsset::GetNormalTex()
	{
		return m_Material->GetTexture<Texture2D>(s_NormalMap);
	}

	void MaterialAsset::SetAlbedoTex(AssetHandle handle)
	{
		m_AlbedoTexture = handle;
		SetTextureInternal(s_AlbedoMap, m_AlbedoTexture);
	}

	void MaterialAsset::SetNormalTex(AssetHandle handle)
	{
		m_NormalTexture = handle;
		SetTextureInternal(s_NormalMap, m_NormalTexture);
	}

	void MaterialAsset::SetMetallicRoughnessTex(AssetHandle handle)
	{
		m_MetallicRoughnessTexture = handle;
		SetTextureInternal(s_MetallicRoughnessMap, m_MetallicRoughnessTexture);
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