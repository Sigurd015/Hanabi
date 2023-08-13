#pragma once
#include "AssetManagerBase.h"
#include "AssetMetadata.h"

#include <map>

namespace Hanabi
{
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		~EditorAssetManager();
		virtual Ref<Asset> GetAsset(AssetHandle handle) const override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

		AssetType GetAssetTypeFromExtension(const std::string& extension);

		AssetHandle ImportAsset(const std::filesystem::path& filepath);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		// TODO: memory-only assets
	};
}