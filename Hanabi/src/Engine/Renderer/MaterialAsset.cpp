#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	static const std::string s_AlbedoMap= "u_AlbedoTex";
	static const std::string s_NormalMap = "u_NormalTex";
	static const std::string s_MetalnessMap = "u_MetalnessTex";
	static const std::string s_RoughnessMap = "u_RoughnessTex";

	MaterialAsset::MaterialAsset() :m_Material(CreateRef<Material>(Renderer::GetDefaultShader()))
	{
		m_Material->SetTexture<Texture2D>(s_AlbedoMap, Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>(s_MetalnessMap, Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>(s_RoughnessMap, Renderer::GetTexture<Texture2D>("White"));
		m_Material->SetTexture<Texture2D>(s_NormalMap, Renderer::GetTexture<Texture2D>("Black"));
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

	Ref<Texture2D> MaterialAsset::GetMetalnessTex()
	{
		return m_Material->GetTexture<Texture2D>(s_MetalnessMap);
	}

	Ref<Texture2D> MaterialAsset::GetRoughnessTex()
	{
		return m_Material->GetTexture<Texture2D>(s_RoughnessMap);
	}

	void MaterialAsset::SetAlbedoTex(AssetHandle handle)
	{
		m_AlbedoTexture = handle;
		SetTextureInternal(s_AlbedoMap, m_AlbedoTexture);
	}

	void MaterialAsset::SetMetalnessTex(AssetHandle handle)
	{
		m_MetalnessTexture = handle;
		SetTextureInternal(s_MetalnessMap, m_MetalnessTexture);
	}

	void MaterialAsset::SetRoughnessTex(AssetHandle handle)
	{
		m_RoughnessTexture = handle;
		SetTextureInternal(s_RoughnessMap, m_RoughnessTexture);
	}

	void MaterialAsset::SetNormalTex(AssetHandle handle)
	{
		m_NormalTexture = handle;
		SetTextureInternal(s_NormalMap, m_NormalTexture);
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