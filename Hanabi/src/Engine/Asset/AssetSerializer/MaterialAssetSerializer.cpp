#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
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

	std::string MaterialAssetSerializer::SerializeToYAML(Ref<MaterialAsset> materialAsset) const
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Material" << YAML::Value;
			{
				out << YAML::BeginMap;// Material
				out << YAML::Key << "DiffuseTextureHandle" << YAML::Value << materialAsset->GetDiffuseHandle();
				out << YAML::Key << "SpecularTextureHandle" << YAML::Value << materialAsset->GetSpecularHandle();
				out << YAML::Key << "NormalTextureHandle" << YAML::Value << materialAsset->GetNormalHandle();
				out << YAML::Key << "UseNormalMap" << YAML::Value << materialAsset->IsUsingNormalMap();
				out << YAML::EndMap; // Material
			}
			out << YAML::EndMap; // Root
		}

		return std::string(out.c_str());
	}

	bool MaterialAssetSerializer::DeserializeFromYAML(const std::string& yamlString, Ref<MaterialAsset>& targetMaterialAsset) const
	{
		YAML::Node root = YAML::Load(yamlString);
		YAML::Node materialNode = root["Material"];

		AssetHandle diffuseHandle = materialNode["DiffuseTextureHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(diffuseHandle))
			targetMaterialAsset->SetDiffuse(diffuseHandle);
		AssetHandle specularHandle = materialNode["SpecularTextureHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(specularHandle))
			targetMaterialAsset->SetSpecular(specularHandle);
		AssetHandle normalHandle = materialNode["NormalTextureHandle"].as<uint64_t>();
		if (AssetManager::IsAssetHandleValid(normalHandle))
			targetMaterialAsset->SetNormal(normalHandle);

		targetMaterialAsset->SetUseNormalMap(materialNode["UseNormalMap"].as<bool>());

		return true;
	}
}