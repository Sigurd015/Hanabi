#pragma once
#include "Material.h"

namespace Hanabi
{
	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset();
		MaterialAsset(Ref<Material> material);
		~MaterialAsset() = default;

		static Ref<MaterialAsset> Create() { return CreateRef<MaterialAsset>(); }

		Ref<Texture2D> GetAlbedoTex();
		Ref<Texture2D> GetNormalTex();
		Ref<Texture2D> GetMetalnessTex();
		Ref<Texture2D> GetRoughnessTex();

		void SetAlbedoTex(AssetHandle handle);
		void SetNormalTex(AssetHandle handle);
		void SetMetallicRoughnessTex(AssetHandle handle);

		void SetAlbedo(const glm::vec3& albedo) { m_Albedo = albedo; }
		void SetEmission(float emission) { m_Emission = emission; }
		void SetMetalness(float metalness) { m_Metalness = metalness; }
		void SetRoughness(float roughness) { m_Roughness = roughness; }
		void SetUseNormalMap(bool value) { m_UseNormalMap = value; }

		AssetHandle GetAlbedoTexHandle() const { return m_AlbedoTexture; }
		AssetHandle GetNormalTexHandle() const { return m_NormalTexture; }
		AssetHandle GetMetallicRoughnessTexHandle() const { return m_MetallicRoughnessTexture; }

		glm::vec3& GetAlbedo() { return m_Albedo; }
		float& GetEmission() { return m_Emission; }
		float& GetMetalness() { return m_Metalness; }
		float& GetRoughness() { return m_Roughness; }
		bool IsUsingNormalMap() const { return m_UseNormalMap; }

		Ref<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Ref<Material> material) { m_Material = material; }

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		void SetTextureInternal(const std::string& name, AssetHandle handle, const std::string& defaultTexName = "White");

		Ref<Material> m_Material;

		AssetHandle m_AlbedoTexture = 0;
		AssetHandle m_MetallicRoughnessTexture = 0;
		AssetHandle m_NormalTexture = 0;

		bool m_UseNormalMap = false;
		glm::vec3 m_Albedo = glm::vec3(1.0f);
		float m_Metalness = 0.0f;
		float m_Roughness = 0.0f;
		float m_Emission = 0.0f;
	};
}