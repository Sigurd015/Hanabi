#pragma once
#include "Material.h"

namespace Hanabi
{
	class MaterialAsset
	{
	public:
		// Phong Lighting Model for now
		enum class TextureType
		{
			Diffuse = 0,
			Specular = 1,
			Normal = 2,
		};

		MaterialAsset();
		MaterialAsset(Ref<Material> material);
		MaterialAsset(const Ref<MaterialAsset>& other);
		~MaterialAsset() = default;

		//TODO : For now(Phong lighting model) we only need diffuse and specular textures
		Ref<Texture2D> GetDiffuse() { return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Diffuse)); }
		void SetDiffuse(Ref<Texture2D> texture) { SetTexture(texture, static_cast<uint32_t>(TextureType::Diffuse)); }
		void ClearDiffuse() { ClearTexture(static_cast<uint32_t>(TextureType::Diffuse)); }

		Ref<Texture2D> GetSpecular() { return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Specular)); }
		void SetSpecular(Ref<Texture2D> texture) { SetTexture(texture, static_cast<uint32_t>(TextureType::Specular)); }
		void ClearSpecular() { ClearTexture(static_cast<uint32_t>(TextureType::Specular)); }

		Ref<Texture2D> GetNormal() { return m_Material->GetTexture(static_cast<uint32_t>(TextureType::Normal)); }
		void SetNormal(Ref<Texture2D> texture) { SetTexture(texture, static_cast<uint32_t>(TextureType::Normal)); }
		bool IsUsingNormalMap() { return m_UseNormalMap; }
		void SetUseNormalMap(bool value) { m_UseNormalMap = value; }
		void ClearNormal() { ClearTexture(static_cast<uint32_t>(TextureType::Normal)); }

		Ref<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Ref<Material> material) { m_Material = material; }
	private:
		void SetTexture(Ref<Texture2D> texture, uint32_t index);
		void ClearTexture(uint32_t index);

		Ref<Material> m_Material;
		bool m_UseNormalMap = false;
	};
}