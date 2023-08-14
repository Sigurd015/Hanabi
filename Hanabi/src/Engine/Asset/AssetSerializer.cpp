#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"
#include "AssetManager.h"

#include <stb_image.h>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = LoadTexture2D(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (asset)
			return true;
		return false;
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		std::string pathStr = path.string();
		data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);

		if (data.Data == nullptr)
		{
			HNB_CORE_ERROR("TextureSerializer::LoadTexture2D - Could not load texture from filepath: {}", path.string());
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

	bool MeshSourceSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = LoadMeshSource(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (asset)
			return true;
		return false;
	}

	Ref<MeshSource> MeshSourceSerializer::LoadMeshSource(const std::filesystem::path& path)
	{
		Assimp::Importer importer;
		auto pAssimpScene = importer.ReadFile(path.string(),
			aiProcess_ConvertToLeftHanded |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_CalcTangentSpace
		);

		std::vector<Vertex> vertices;
		std::vector<Index> indices;

		if (pAssimpScene && !(pAssimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && pAssimpScene->HasMeshes())
		{
			auto pMesh = pAssimpScene->mMeshes[0];
			uint32_t numVertices = pMesh->mNumVertices;
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				aiVector3D vertex = pMesh->mVertices[i];
				aiVector3D normal = pMesh->mNormals[i];
				aiVector3D texCoord = pMesh->mTextureCoords[0][i];
				aiVector3D tangent = pMesh->mTangents[i];
				aiVector3D bitangent = pMesh->mBitangents[i];
				vertices.push_back({
					{vertex.x, vertex.y, vertex.z},
					{normal.x, normal.y, normal.z},
					{tangent.x,tangent.y,tangent.z},
					{bitangent.x,bitangent.y,bitangent.z},
					{texCoord.x, texCoord.y},
					});
			}

			for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
			{
				const auto& face = pMesh->mFaces[i];
				indices.push_back({ face.mIndices[0],face.mIndices[1],face.mIndices[2] });
			}

			importer.FreeScene();

			return CreateRef<MeshSource>(vertices, indices);
		}
		return nullptr;
	}
}