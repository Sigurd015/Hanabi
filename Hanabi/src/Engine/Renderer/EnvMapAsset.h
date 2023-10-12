#pragma once
#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"

namespace Hanabi
{
	class EnvMapAsset : public Asset
	{
	public:
		EnvMapAsset() = default;

		AssetHandle GetRadianceMapHandle() const { return m_RadianceMapHandle; }
		AssetHandle GetIrradianceMapHandle() const { return m_IrradianceMapHandle; }

		void SetRadianceMapHandle(AssetHandle handle) { m_RadianceMapHandle = handle; Invalidate(); }
		void SetIrradianceMapHandle(AssetHandle handle) { m_IrradianceMapHandle = handle; Invalidate(); }

		Ref<TextureCube> GetRadianceMap() const;
		Ref<TextureCube> GetIrradianceMap() const;

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		void Invalidate();

		AssetHandle m_RadianceMapHandle;
		AssetHandle m_IrradianceMapHandle;
	};
}