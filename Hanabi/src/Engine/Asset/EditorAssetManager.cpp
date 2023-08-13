#include "hnbpch.h"
#include "AssetManager.h"
#include "AssetImporter.h"
#include "EditorAssetManager.h"
#include "AssetExtensions.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}

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
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
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

	AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle; // generate new handle
		AssetMetadata metadata;
		metadata.FilePath = filepath;
		metadata.Type = GetAssetTypeFromExtension(filepath.extension().string());
		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		asset->Handle = handle;
		if (asset)
		{
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
			SerializeAssetRegistry();
			return handle;
		}
		return 0;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
	{
		// 1. check if handle is valid
		if (!IsAssetHandleValid(handle))
			return nullptr;

		// 2. check if asset needs load (and if so, load)
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			// load asset
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
			{
				// import failed
				HNB_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");
			}
		}
		// 3. return asset
		return asset;
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
			metadata.FilePath = node["FilePath"].as<std::string>();
			metadata.Type = Utils::AssetTypeFromString(node["Type"].as<std::string>());
		}

		return true;
	}
}
