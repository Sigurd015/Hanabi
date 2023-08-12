#pragma once
#include "Engine/Core/Base.h"
#include "AssetMetadata.h"
#include "Asset.h"

namespace Hanabi
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}