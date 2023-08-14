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
		s_Serializers[AssetType::MeshSource] = CreateScope<MeshSourceSerializer>();
	}

	void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HNB_CORE_WARN("There's currently no Serializer for assets of type {0}", metadata.FilePath.stem().string());
			return;
		}

		s_Serializers[metadata.Type]->Serialize(metadata, asset);
	}

	void AssetImporter::Serialize(const Ref<Asset>& asset)
	{
		const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(asset->Handle);
		Serialize(metadata, asset);
	}

	bool AssetImporter::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HNB_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
			return false;
		}

		return s_Serializers.at(metadata.Type)->TryLoadData(metadata, asset);
	}
}