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

		Ref<Texture2D> GetDiffuse();
		Ref<Texture2D> GetSpecular();
		Ref<Texture2D> GetNormal();

		void SetDiffuse(AssetHandle handle);
		void SetSpecular(AssetHandle handle);
		void SetNormal(AssetHandle handle);
		void SetUseNormalMap(bool value) { m_UseNormalMap = value; }

		AssetHandle GetDiffuseHandle() const { return m_DiffuseTexture; }
		AssetHandle GetSpecularHandle() const { return m_SpecularTexture; }
		AssetHandle GetNormalHandle() const { return m_NormalTexture; }
		bool IsUsingNormalMap() const { return m_UseNormalMap; }

		Ref<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Ref<Material> material) { m_Material = material; }

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		void SetTexture(const std::string& name, const Ref<Texture2D>& texture);

		Ref<Material> m_Material;

		AssetHandle m_DiffuseTexture = 0;
		AssetHandle m_SpecularTexture = 0;
		AssetHandle m_NormalTexture = 0;

		bool m_UseNormalMap = false;
	};
}