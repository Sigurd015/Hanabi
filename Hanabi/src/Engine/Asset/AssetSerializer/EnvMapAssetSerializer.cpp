#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	static std::string s_DefaultEnvMaplYAML = R"(EnvMap:
  TopHandle: 0
  BottomHandle: 0
  LeftHandle: 0
  RightHandle: 0
  FrontHandle: 0
  BackHandle: 0)";

	void EnvMapAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<EnvMapAsset> envMap = std::static_pointer_cast<EnvMapAsset>(asset);
		std::string yamlString = SerializeToYAML(envMap);
		std::ofstream fout(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		fout << yamlString;
	}

	bool EnvMapAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		std::ifstream stream(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (!stream.is_open())
			return false;

		std::stringstream strStream;
		strStream << stream.rdbuf();

		Ref<EnvMapAsset> envMap = CreateRef<EnvMapAsset>();
		bool success = DeserializeFromYAML(strStream.str(), envMap);
		if (!success)
			return false;

		envMap->Invalidate();

		asset = envMap;
		asset->Handle = metadata.Handle;
		return true;
	}

	void EnvMapAssetSerializer::SerializeToYAML(const std::filesystem::path& path)
	{
		std::ofstream fout(path);
		fout << s_DefaultEnvMaplYAML;
	}

	std::string EnvMapAssetSerializer::SerializeToYAML(Ref<EnvMapAsset> envMapAsset)
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "EnvMap" << YAML::Value;
			{
				out << YAML::BeginMap;// EnvMap
				out << YAML::Key << "TopHandle" << YAML::Value << envMapAsset->GetTopHandle();
				out << YAML::Key << "BottomHandle" << YAML::Value << envMapAsset->GetBottomHandle();
				out << YAML::Key << "LeftHandle" << YAML::Value << envMapAsset->GetLeftHandle();
				out << YAML::Key << "RightHandle" << YAML::Value << envMapAsset->GetRightHandle();
				out << YAML::Key << "FrontHandle" << YAML::Value << envMapAsset->GetFrontHandle();
				out << YAML::Key << "BackHandle" << YAML::Value << envMapAsset->GetBackHandle();
				out << YAML::EndMap; // EnvMap
			}
			out << YAML::EndMap; // Root
		}

		return std::string(out.c_str());
	}

	bool EnvMapAssetSerializer::DeserializeFromYAML(const std::string& yamlString, Ref<EnvMapAsset>& targetEnvMapAsset)
	{
		YAML::Node root = YAML::Load(yamlString);
		YAML::Node envMapNode = root["EnvMap"];

		TextureSpecification spec;
		std::vector<Buffer> textureData;

		AssetHandle topHandle = envMapNode["TopHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(topHandle))
			targetEnvMapAsset->SetTopHandle(topHandle);

		AssetHandle bottomHandle = envMapNode["BottomHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(bottomHandle))
			targetEnvMapAsset->SetBottomHandle(bottomHandle);

		AssetHandle leftHandle = envMapNode["LeftHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(leftHandle))
			targetEnvMapAsset->SetLeftHandle(leftHandle);

		AssetHandle rightHandle = envMapNode["RightHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(rightHandle))
			targetEnvMapAsset->SetRightHandle(rightHandle);

		AssetHandle frontHandle = envMapNode["FrontHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(frontHandle))
			targetEnvMapAsset->SetFrontHandle(frontHandle);

		AssetHandle backHandle = envMapNode["BackHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(backHandle))
			targetEnvMapAsset->SetBackHandle(backHandle);

		return true;
	}
}