#include "hnbpch.h"
#include "AssetManager.h"
#include "Engine/Asset/AssetImporter.h"
#include "EditorAssetManager.h"
#include "Engine/Asset/AssetExtensions.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	static AssetMetadata s_NullMetadata;

	EditorAssetManager::EditorAssetManager()
	{
		AssetImporter::Init();

		DeserializeAssetRegistry();
	}

	EditorAssetManager::~EditorAssetManager()
	{
		//SerializeAssetRegistry();
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return IsMemoryAsset(handle) || GetMetadata(handle).IsValid();
	}

	bool EditorAssetManager::IsMemoryAsset(AssetHandle handle) const
	{
		return m_MemoryAssets.find(handle) != m_MemoryAssets.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	bool EditorAssetManager::ReloadData(AssetHandle assetHandle)
	{
		auto& metadata = GetMetadataInternal(assetHandle);
		if (!metadata.IsValid())
		{
			HNB_CORE_ERROR("Trying to reload invalid asset");
			return false;
		}

		Ref<Asset> asset;
		metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
		if (metadata.IsDataLoaded)
		{
			m_LoadedAssets[assetHandle] = asset;
		}
		return metadata.IsDataLoaded;
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return AssetType::None;
		return m_AssetRegistry.at(handle).Type;
	}

	AssetType EditorAssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			return AssetType::None;

		return s_AssetExtensionMap.at(extension.c_str());
	}

	AssetType EditorAssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		return GetAssetTypeFromExtension(path.extension().string());
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		std::filesystem::path path = GetRelativePath(filepath);

		if (auto& metadata = GetMetadata(path); metadata.IsValid())
			return metadata.Handle;

		AssetType type = GetAssetTypeFromPath(path);
		if (type == AssetType::None)
			return 0;

		AssetMetadata metadata;
		metadata.Handle = AssetHandle();
		metadata.FilePath = path;
		metadata.Type = type;
		m_AssetRegistry[metadata.Handle] = metadata;

		//TODO: Call this at ~EditorAssetManager is the best way, but it will cause crash when close the editor
		SerializeAssetRegistry();

		return metadata.Handle;
	}

	void EditorAssetManager::AddMemoryOnlyAsset(Ref<Asset> asset)
	{
		AssetMetadata metadata;
		metadata.Handle = asset->Handle;
		metadata.IsDataLoaded = true;
		metadata.Type = asset->GetAssetType();
		metadata.IsMemoryAsset = true;
		m_AssetRegistry[metadata.Handle] = metadata;

		m_MemoryAssets[asset->Handle] = asset;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const std::filesystem::path& filepath)
	{
		const auto relativePath = GetRelativePath(filepath);

		for (auto& [handle, metadata] : m_AssetRegistry)
		{
			if (metadata.FilePath == relativePath)
				return metadata;
		}

		return s_NullMetadata;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const Ref<Asset>& asset)
	{
		return GetMetadata(asset->Handle);
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return nullptr;

		if (IsMemoryAsset(handle))
			return m_MemoryAssets[handle];

		auto& metadata = GetMetadataInternal(handle);
		if (!metadata.IsValid())
			return nullptr;

		Ref<Asset> asset = nullptr;
		if (!metadata.IsDataLoaded)
		{
			metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
			if (!metadata.IsDataLoaded)
				return nullptr;

			m_LoadedAssets[handle] = asset;
		}
		else
		{
			asset = m_LoadedAssets[handle];
		}

		return asset;
	}

	std::filesystem::path EditorAssetManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path relativePath = filepath.lexically_normal();
		std::string temp = filepath.string();
		if (temp.find(Project::GetAssetDirectory().string()) != std::string::npos)
		{
			relativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
			if (relativePath.empty())
			{
				relativePath = filepath.lexically_normal();
			}
		}
		return relativePath;
	}

	std::filesystem::path EditorAssetManager::GetFileSystemPath(const AssetMetadata& metadata)
	{
		return Project::GetAssetDirectory() / metadata.FilePath;
	}

	std::filesystem::path EditorAssetManager::GetFileSystemPath(AssetHandle handle)
	{
		return GetFileSystemPathString(GetMetadata(handle));
	}

	std::string EditorAssetManager::GetFileSystemPathString(const AssetMetadata& metadata)
	{
		return GetFileSystemPath(metadata).string();
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "AssetRegistry" << YAML::Value;

			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : m_AssetRegistry)
			{
				if (metadata.IsMemoryAsset)
					continue;

				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				std::string filepathStr = metadata.FilePath.generic_string();
				out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
				out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(metadata.Type);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		} catch (YAML::ParserException e)
		{
			HNB_CORE_ERROR("Failed to load project file '{0}'\n {1}", path, e.what());
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;

		for (const auto& node : rootNode)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];
			metadata.Handle = handle;
			metadata.FilePath = node["FilePath"].as<std::string>();
			metadata.Type = Utils::AssetTypeFromString(node["Type"].as<std::string>());
		}

		return true;
	}

	AssetMetadata& EditorAssetManager::GetMetadataInternal(AssetHandle handle)
	{
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}
}
