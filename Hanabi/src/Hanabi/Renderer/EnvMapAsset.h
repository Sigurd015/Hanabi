#pragma once
#include "Hanabi/Asset/Asset.h"
#include "Hanabi/Renderer/Texture.h"
#include "Hanabi/Renderer/Renderer.h"

namespace Hanabi
{
	class EnvMapAsset : public Asset
	{
	public:
		Ref<TextureCube> RadianceMap;
		Ref<TextureCube> IrradianceMap;

		EnvMapAsset() = default;
		EnvMapAsset(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap)
		{}

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	};
}