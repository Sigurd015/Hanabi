#pragma once
#include "AssetManagerBase.h"
#include "Hanabi/Asset/AssetMetadata.h"
#include "Hanabi/Utils/Hash.h"

#include <map>

namespace Hanabi
{
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		~EditorAssetManager();
		virtual Ref<Asset> GetAsset(AssetHandle handle) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsMemoryAsset(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual bool ReloadData(AssetHandle assetHandle) override;

		virtual AssetType GetAssetType(AssetHandle handle) const override;

		AssetType GetAssetTypeFromExtension(const std::string& extension);
		AssetType GetAssetTypeFromPath(const std::filesystem::path& path);

		AssetHandle ImportAsset(const std::filesystem::path& filepath);
		void RemoveAsset(AssetHandle handle);

		virtual void AddMemoryOnlyAsset(Ref<Asset> asset) override;

		template<typename TAsset, typename... TArgs>
		AssetHandle CreateNamedMemoryOnlyAsset(const std::string& name, TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = TAsset::Create(std::forward<TArgs>(args)...);
			asset->Handle = Hash::GenerateFNVHash(name);

			AssetMetadata metadata;
			metadata.Handle = asset->Handle;
			metadata.FilePath = name;
			metadata.IsDataLoaded = true;
			metadata.Type = TAsset::GetStaticType();
			metadata.IsMemoryAsset = true;

			m_AssetRegistry[metadata.Handle] = metadata;

			m_MemoryAssets[asset->Handle] = asset;
			return asset->Handle;
		}

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const AssetMetadata& GetMetadata(const std::filesystem::path& filepath);
		const AssetMetadata& GetMetadata(const Ref<Asset>& asset);

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		std::filesystem::path GetFileSystemPath(const AssetMetadata& metadata);
		std::filesystem::path GetFileSystemPath(AssetHandle handle);
		std::string GetFileSystemPathString(const AssetMetadata& metadata);
		std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:
		AssetMetadata& GetMetadataInternal(AssetHandle handle);

		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
		AssetMap m_MemoryAssets;
	};
}