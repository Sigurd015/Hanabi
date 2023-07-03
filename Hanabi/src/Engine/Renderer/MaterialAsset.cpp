#include "hnbpch.h"
#include "MaterialAsset.h"
#include "Renderer.h"

namespace Hanabi
{
	MaterialAsset::MaterialAsset()
	{
		*this = Renderer::GetDefaultMaterialAsset();
	}

	MaterialAsset::MaterialAsset(Ref<Material> material)
	{
		m_Material = Material::Copy(material);
	}

	MaterialAsset::MaterialAsset(const Ref<MaterialAsset>& other)
	{
		m_Material = Material::Copy(other->GetMaterial());
	}

	void MaterialAsset::SetTexture(Ref<Texture2D> texture, uint32_t index)
	{
		m_Material->SetTexture(texture, index);
	}

	void MaterialAsset::ClearTexture(uint32_t index)
	{
		m_Material->SetTexture(Renderer::GetTexture("White"), index);
	}
}