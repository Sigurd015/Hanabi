#pragma once
#include "Engine/Core/Base.h"
#include "AssetMetadata.h"
#include "Asset.h"
#include "AssetSerializer.h"

namespace Hanabi
{
	class AssetImporter
	{
	public:
		static void Init();
		static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);
		static void Serialize(const Ref<Asset>& asset);

		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}