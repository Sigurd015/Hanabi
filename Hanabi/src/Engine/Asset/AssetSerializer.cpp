#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	void MaterialAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		auto path = Project::GetMaterialPath();
		Ref<MaterialAsset> material = std::dynamic_pointer_cast<MaterialAsset>(asset);
		std::string yamlString = SerializeToYAML(material);
		std::ofstream fout(path);
		fout << yamlString.c_str();
	}

	Ref<Asset> MaterialAssetSerializer::ImportData(AssetHandle handle, const AssetMetadata& metadata) const
	{
		return DeserializeFromYAML(Project::GetAssetDirectory() / metadata.FilePath);
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
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		return std::string(out.c_str());
	}

	Ref<MaterialAsset> MaterialAssetSerializer::DeserializeFromYAML(const std::filesystem::path& path) const
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		} catch (YAML::ParserException e)
		{
			HNB_CORE_ERROR("Failed to load material file '{0}'\n     {1}", path, e.what());
			return nullptr;
		}

		auto projectNode = data["Material"];
		Ref<MaterialAsset> material = CreateRef<MaterialAsset>();
		material->SetDiffuse(projectNode["DiffuseTextureHandle"].as<uint64_t>());
		material->SetSpecular(projectNode["SpecularTextureHandle"].as<uint64_t>());
		material->SetNormal(projectNode["NormalTextureHandle"].as<uint64_t>());
		material->SetUseNormalMap(projectNode["UseNormalMap"].as<bool>());

		return material;
	}

	Ref<Asset> TextureSerializer::ImportData(AssetHandle handle, const AssetMetadata& metadata) const
	{
		return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		std::string pathStr = path.string();
		data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 0);

		if (data.Data == nullptr)
		{
			HNB_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();
		return texture;
	}
}