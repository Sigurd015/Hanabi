#include "hnbpch.h"
#include "AssetImporter.h"
#include "Engine/Project/Project.h"

#include <map>

namespace Hanabi
{
	static std::unordered_map<AssetType, Scope<AssetSerializer>> s_Serializers;

	void AssetImporter::Init()
	{
		s_Serializers.clear();
		s_Serializers[AssetType::Texture2D] = CreateScope<TextureSerializer>();
		s_Serializers[AssetType::Material] = CreateScope<MaterialAssetSerializer>();
	}

	void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HNB_CORE_WARN("There's currently no Serializer for assets of type {0}", metadata.FilePath.stem().string());
			return;
		}

		s_Serializers[asset->GetType()]->Serialize(metadata, asset);
	}

	void AssetImporter::Serialize(const Ref<Asset>& asset)
	{
		const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(asset->Handle);
		Serialize(metadata, asset);
	}

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HNB_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_Serializers.at(metadata.Type)->ImportData(handle, metadata);
	}
}