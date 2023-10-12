#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	static std::string s_DefaultEnvMaplYAML = R"(EnvMap:
  RadianceMapHandle: 0
  IrradianceMapHandle: 0)";

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
				out << YAML::Key << "RadianceMapHandle" << YAML::Value << envMapAsset->GetRadianceMapHandle();
				out << YAML::Key << "IrradianceMapHandle" << YAML::Value << envMapAsset->GetIrradianceMapHandle();
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

		AssetHandle radianceMapHandle = envMapNode["RadianceMapHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(radianceMapHandle))
			targetEnvMapAsset->SetRadianceMapHandle(radianceMapHandle);

		AssetHandle irradianceMapHandle = envMapNode["IrradianceMapHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(irradianceMapHandle))
			targetEnvMapAsset->SetIrradianceMapHandle(irradianceMapHandle);

		return true;
	}
}