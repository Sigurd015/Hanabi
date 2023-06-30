#pragma once
#include "Material.h"

namespace Hanabi
{
	class MaterialAsset
	{
	public:
		MaterialAsset();
		MaterialAsset(Ref<Material> material);
		MaterialAsset(const Ref<MaterialAsset>& other);
		~MaterialAsset() = default;

		//TODO : For now(Phong lighting model) we only need diffuse and specular textures
		Ref<Texture2D> GetDiffuse();
		void SetDiffuse(Ref<Texture2D> texture);
		void ClearDiffuse();

		Ref<Texture2D> GetSpecular();
		void SetSpecular(Ref<Texture2D> texture);
		void ClearSpecular();

		Ref<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Ref<Material> material) { m_Material = material; }
	private:
		Ref<Material> m_Material;
	};
}