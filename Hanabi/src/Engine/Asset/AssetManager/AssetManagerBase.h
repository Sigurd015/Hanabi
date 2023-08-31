#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Asset/Asset.h"

#include <map>

namespace Hanabi
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual void AddMemoryOnlyAsset(Ref<Asset> asset) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsMemoryAsset(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual bool ReloadData(AssetHandle assetHandle) = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}