#pragma once
#include "Engine/Core/Base.h"
#include "Asset.h"

#include <map>

namespace Hanabi
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) const = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}