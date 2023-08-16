#pragma once
#include "AssetManagerBase.h"

#include "Engine/Project/Project.h"

namespace Hanabi
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Project::GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle)
		{
			return Project::GetAssetManager()->IsAssetHandleValid(handle);
		}

		static bool ReloadData(AssetHandle assetHandle)
		{ 
			return Project::GetAssetManager()->ReloadData(assetHandle); 
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetAssetManager()->IsAssetLoaded(handle);
		}

		static AssetType GetAssetType(AssetHandle handle)
		{
			return Project::GetAssetManager()->GetAssetType(handle);
		}

		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = CreateRef<TAsset>(std::forward<TArgs>(args)...);
			asset->Handle = AssetHandle();

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}
	};
}