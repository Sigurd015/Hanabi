#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "Engine/Renderer/MaterialAsset.h"


namespace Hanabi
{
	class AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;

		// AssetMetadata filepath is relative to project asset directory
		virtual Ref<Asset> ImportData(AssetHandle handle, const AssetMetadata& metadata) const = 0;
	};

	class MaterialAssetSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;

		virtual Ref<Asset> ImportData(AssetHandle handle, const AssetMetadata& metadata) const override;

	private:
		std::string SerializeToYAML(Ref<MaterialAsset> materialAsset) const;
		Ref<MaterialAsset> DeserializeFromYAML(const std::filesystem::path& path) const;
	};

	class TextureSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}

		virtual Ref<Asset> ImportData(AssetHandle handle, const AssetMetadata& metadata) const override;

		// Reads file directly from filesystem
		// (i.e. path has to be relative / absolute to working directory)
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}