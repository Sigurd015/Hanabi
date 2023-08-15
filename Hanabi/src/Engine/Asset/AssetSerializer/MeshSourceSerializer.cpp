#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Hanabi
{
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