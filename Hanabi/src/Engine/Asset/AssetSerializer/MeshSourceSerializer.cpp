#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Hanabi
{
	static const uint32_t s_MeshImportFlags =
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_Triangulate;            // Make sure we're triangles
	    //aiProcess_OptimizeGraph |
	    //aiProcess_OptimizeMeshes |          // Batch draws where possible
	    //aiProcess_JoinIdenticalVertices |
	    //aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
	    //aiProcess_ValidateDataStructure   // Validation
	    //aiProcess_GlobalScale              // e.g. convert cm to m for fbx import (and other formats where cm is native)

	bool MeshSourceSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = LoadMeshSource(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (asset)
		{
			asset->Handle = metadata.Handle;
			return true;
		}
		return false;
	}

	Ref<MeshSource> MeshSourceSerializer::LoadMeshSource(const std::filesystem::path& path)
	{
		Ref<MeshSource> meshSource = CreateRef<MeshSource>();

		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		const aiScene* pScene = importer.ReadFile(path.string(), s_MeshImportFlags);

		if (!pScene || !pScene->HasMeshes())
		{
			HNB_CORE_ERROR("Failed to load mesh source: {0}", importer.GetErrorString());
			return nullptr;
		}

		// TODO: Support multiple meshes
		aiMesh* pMesh = pScene->mMeshes[0];
		for (uint32_t i = 0; i < pMesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = { pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z };
			vertex.Normal = { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z };

			if (pMesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { pMesh->mTangents[i].x, pMesh->mTangents[i].y, pMesh->mTangents[i].z };
				vertex.Binormal = { pMesh->mBitangents[i].x, pMesh->mBitangents[i].y, pMesh->mBitangents[i].z };
			}

			if (pMesh->HasTextureCoords(0))
				vertex.TexCoord = { pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y };

			meshSource->m_Vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < pMesh->mNumFaces; i++)
		{
			Index index = { pMesh->mFaces[i].mIndices[0], pMesh->mFaces[i].mIndices[1], pMesh->mFaces[i].mIndices[2] };
			meshSource->m_Indices.push_back(index);
		}

		meshSource->Invalidate();
		return meshSource;
	}
}