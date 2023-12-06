#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Hanabi/Project/Project.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"
#include "Hanabi/Utils/YAMLHelpers.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	static std::string s_DefaultMaterialYAML = R"(Material:
  AlbedoTexHandle: 0
  MetalnessTexHandle: 0
  RoughnessTexHandle: 0
  NormalTexHandle: 0
  AlbedoColor: [0, 0, 0]
  Emission: 0.0
  Metalness: 0.0
  Roughness: 0.0
  UseNormalMap: false)";

	void MaterialAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<MaterialAsset> material = std::static_pointer_cast<MaterialAsset>(asset);
		std::string yamlString = SerializeToYAML(material);
		std::ofstream fout(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		fout << yamlString;
	}

	bool MaterialAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		std::ifstream stream(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (!stream.is_open())
			return false;

		std::stringstream strStream;
		strStream << stream.rdbuf();

		Ref<MaterialAsset> materialAsset = CreateRef<MaterialAsset>();
		bool success = DeserializeFromYAML(strStream.str(), materialAsset);
		if (!success)
			return false;

		asset = materialAsset;
		asset->Handle = metadata.Handle;
		return true;
	}

	void MaterialAssetSerializer::SerializeToYAML(const std::filesystem::path& path)
	{
		std::ofstream fout(path);
		fout << s_DefaultMaterialYAML;
	}

	std::string MaterialAssetSerializer::SerializeToYAML(Ref<MaterialAsset> materialAsset)
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Material" << YAML::Value;
			{
				out << YAML::BeginMap;// Material
				out << YAML::Key << "AlbedoTexHandle" << YAML::Value << materialAsset->GetAlbedoTexHandle();
				out << YAML::Key << "MetalnessTexHandle" << YAML::Value << materialAsset->GetMetalnessTexHandle();
				out << YAML::Key << "RoughnessTexHandle" << YAML::Value << materialAsset->GetRoughnessTexHandle();
				out << YAML::Key << "NormalTexHandle" << YAML::Value << materialAsset->GetNormalTexHandle();
				out << YAML::Key << "AlbedoColor" << YAML::Value << materialAsset->GetAlbedo();
				out << YAML::Key << "Emission" << YAML::Value << materialAsset->GetEmission();
				out << YAML::Key << "Metalness" << YAML::Value << materialAsset->GetMetalness();
				out << YAML::Key << "Roughness" << YAML::Value << materialAsset->GetRoughness();
				out << YAML::Key << "UseNormalMap" << YAML::Value << materialAsset->IsUsingNormalMap();
				out << YAML::EndMap; // Material
			}
			out << YAML::EndMap; // Root
		}

		return std::string(out.c_str());
	}

	bool MaterialAssetSerializer::DeserializeFromYAML(const std::string& yamlString, Ref<MaterialAsset>& targetMaterialAsset)
	{
		YAML::Node root = YAML::Load(yamlString);
		YAML::Node materialNode = root["Material"];

		AssetHandle albedoTexHandle = materialNode["AlbedoTexHandle"].as<uint64_t>();
		targetMaterialAsset->SetAlbedoTex(albedoTexHandle);
		AssetHandle metalnessTexHandle = materialNode["MetalnessTexHandle"].as<uint64_t>();
		targetMaterialAsset->SetMetalnessTex(metalnessTexHandle);
		AssetHandle roughnessTexHandle = materialNode["RoughnessTexHandle"].as<uint64_t>();
		targetMaterialAsset->SetRoughnessTex(roughnessTexHandle);
		AssetHandle normalHandle = materialNode["NormalTexHandle"].as<uint64_t>();
		targetMaterialAsset->SetNormalTex(normalHandle);

		targetMaterialAsset->SetAlbedo(materialNode["AlbedoColor"].as<glm::vec3>());
		targetMaterialAsset->SetEmission(materialNode["Emission"].as<float>());
		targetMaterialAsset->SetMetalness(materialNode["Metalness"].as<float>());
		targetMaterialAsset->SetRoughness(materialNode["Roughness"].as<float>());
		targetMaterialAsset->SetUseNormalMap(materialNode["UseNormalMap"].as<bool>());

		return true;
	}
}