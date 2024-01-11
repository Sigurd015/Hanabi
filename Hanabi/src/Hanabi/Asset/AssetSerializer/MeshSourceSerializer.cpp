#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Hanabi/Project/Project.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"

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
		aiProcess_JoinIdenticalVertices |   // Join identical vertices/ optimize indexing
		aiProcess_Triangulate;            // Make sure we're triangles
	//aiProcess_OptimizeGraph |
	//aiProcess_OptimizeMeshes |          // Batch draws where possible
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

		const aiScene* scene = importer.ReadFile(path.string(), s_MeshImportFlags);

		if (!scene)
		{
			HNB_CORE_ERROR("Failed to load mesh source: {0}", importer.GetErrorString());
			return nullptr;
		}

		// TODO: Support Skeletons and Animations

		if (scene->HasMeshes())
		{
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			meshSource->m_BoundingBox.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
			meshSource->m_BoundingBox.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			// Make sure we have at least one submesh
			meshSource->m_Submeshes.reserve(scene->mNumMeshes);
			for (unsigned m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];

				Submesh& submesh = meshSource->m_Submeshes.emplace_back();
				submesh.BaseVertex = vertexCount;
				submesh.BaseIndex = indexCount;
				submesh.MaterialIndex = mesh->mMaterialIndex;
				submesh.VertexCount = mesh->mNumVertices;
				submesh.IndexCount = mesh->mNumFaces * 3;
				submesh.MeshName = mesh->mName.C_Str();

				vertexCount += mesh->mNumVertices;
				indexCount += submesh.IndexCount;

				HNB_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
				HNB_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

				// Vertices
				auto& aabb = submesh.BoundingBox;
				aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
				aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
					aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
					aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
					aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
					aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
					aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
					aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
						vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

					meshSource->m_Vertices.push_back(vertex);
				}

				// Indices
				for (size_t i = 0; i < mesh->mNumFaces; i++)
				{
					HNB_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
					Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
					meshSource->m_Indices.push_back(index);

					meshSource->m_TriangleCache[m].emplace_back(meshSource->m_Vertices[index.V1 + submesh.BaseVertex], meshSource->m_Vertices[index.V2 + submesh.BaseVertex], meshSource->m_Vertices[index.V3 + submesh.BaseVertex]);
				}
			}
		}

		// Calculate mesh source AABB
		for (const auto& submesh : meshSource->m_Submeshes)
		{
			AABB transformedSubmeshAABB = submesh.BoundingBox;
			glm::vec3 min = glm::vec3(submesh.Transform * glm::vec4(transformedSubmeshAABB.Min, 1.0f));
			glm::vec3 max = glm::vec3(submesh.Transform * glm::vec4(transformedSubmeshAABB.Max, 1.0f));

			meshSource->m_BoundingBox.Min.x = glm::min(meshSource->m_BoundingBox.Min.x, min.x);
			meshSource->m_BoundingBox.Min.y = glm::min(meshSource->m_BoundingBox.Min.y, min.y);
			meshSource->m_BoundingBox.Min.z = glm::min(meshSource->m_BoundingBox.Min.z, min.z);
			meshSource->m_BoundingBox.Max.x = glm::max(meshSource->m_BoundingBox.Max.x, max.x);
			meshSource->m_BoundingBox.Max.y = glm::max(meshSource->m_BoundingBox.Max.y, max.y);
			meshSource->m_BoundingBox.Max.z = glm::max(meshSource->m_BoundingBox.Max.z, max.z);
		}

		// Materials
		if (scene->HasMaterials())
		{
			meshSource->m_Materials.resize(scene->mNumMaterials);

			HNB_CORE_INFO("---- Materials - {0} ----", path);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				// TODO: May generate the AssetHandle by hashing the material name
				// So that we can make the AssetHandle consistent across different imports
				auto aiMaterialName = aiMaterial->GetName();
				HNB_CORE_INFO("--- Name Hash --- {0}", Utils::Hash::GenerateFNVHash(aiMaterialName.data));
				meshSource->m_Materials[i] = AssetManager::CreateMemoryOnlyAsset<MaterialAsset>();

				Ref<MaterialAsset> material = AssetManager::GetAsset<MaterialAsset>(meshSource->m_Materials[i]);

				HNB_CORE_INFO("  {0} (Index = {1})", aiMaterialName.data, i);

				aiString aiTexPath;

				glm::vec3 albedoColor(0.8f);
				float emission = 0.0f;
				aiColor3D aiColor, aiEmission;
				if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
					albedoColor = { aiColor.r, aiColor.g, aiColor.b };

				if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS)
					emission = aiEmission.r;

				material->SetAlbedo(albedoColor);
				material->SetEmission(emission);

				float roughness, metalness;
				if (aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) != aiReturn_SUCCESS)
					roughness = 0.5f; // Default value

				if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS)
					metalness = 0.0f;

				// Physically realistic materials are either metal (1.0) or not (0.0)
				// Blender uses 0.5 as a default which seems wrong - materials are either metal or they are not.
				if (metalness < 0.9f)
					metalness = 0.0f;
				else
					metalness = 1.0f;

				material->SetRoughness(roughness);
				material->SetMetalness(metalness);

				HNB_CORE_INFO("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
				HNB_CORE_INFO("    ROUGHNESS = {0}", roughness);
				HNB_CORE_INFO("    METALNESS = {0}", metalness);

				// Albedo
				bool hasAlbedoMap = aiMaterial->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &aiTexPath) == AI_SUCCESS;
				if (!hasAlbedoMap)
				{
					// no PBR base color. Try old-school diffuse
					hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
				}
				if (hasAlbedoMap)
				{
					TextureSpecification spec;
					// TODO: Support SRGB
					//spec.Format = ImageFormat::SRGB;
					Buffer buffer;
					if (auto aiTexEmbedded = scene->GetEmbeddedTexture(aiTexPath.C_Str()))
					{
						spec.Width = aiTexEmbedded->mWidth;
						spec.Height = aiTexEmbedded->mHeight;
						buffer = Buffer(aiTexEmbedded->pcData, spec.Width * spec.Height);
					}
					else
					{
						auto parentPath = path.parent_path();
						parentPath /= aiTexPath.C_Str();
						HNB_CORE_INFO("    Albedo map path = {0}", parentPath.string());
						buffer = TextureSerializer::ToBufferFromFile(parentPath, spec);
					}
					AssetHandle textureHandle = AssetManager::CreateMemoryOnlyAsset<Texture2D>(spec, buffer);
					material->SetAlbedoTex(textureHandle);
				}

				// Normal
				bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
				if (hasNormalMap)
				{
					TextureSpecification spec;
					Buffer buffer;
					if (auto aiTexEmbedded = scene->GetEmbeddedTexture(aiTexPath.C_Str()))
					{
						spec.Format = ImageFormat::RGBA;
						spec.Width = aiTexEmbedded->mWidth;
						spec.Height = aiTexEmbedded->mHeight;
						buffer = Buffer(aiTexEmbedded->pcData, spec.Width * spec.Height);
					}
					else
					{
						auto parentPath = path.parent_path();
						parentPath /= aiTexPath.C_Str();
						HNB_CORE_INFO("    Normal map path = {0}", parentPath.string());
						buffer = TextureSerializer::ToBufferFromFile(parentPath, spec);
					}
					AssetHandle textureHandle = AssetManager::CreateMemoryOnlyAsset<Texture2D>(spec, buffer);
					material->SetNormalTex(textureHandle);
					material->SetUseNormalMap(true);
				}

				// Roughness and Metalness
				{
					aiString aiRoughnessTexPath;
					aiString aiMetalnessTexPath;
					bool hasRoughnessMap = aiMaterial->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &aiRoughnessTexPath) == AI_SUCCESS;
					bool hasMetalnessMap = aiMaterial->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &aiMetalnessTexPath) == AI_SUCCESS;
					if (aiRoughnessTexPath != aiMetalnessTexPath)
					{
						HNB_CORE_ERROR("    Roughness and Metalness map must be the same");
					}
					else
					{
						if (hasRoughnessMap)
						{
							TextureSpecification spec;
							Buffer buffer;
							if (auto aiTexEmbedded = scene->GetEmbeddedTexture(aiRoughnessTexPath.C_Str()))
							{
								spec.Format = ImageFormat::RGBA;
								spec.Width = aiTexEmbedded->mWidth;
								spec.Height = aiTexEmbedded->mHeight;
								buffer = Buffer(aiTexEmbedded->pcData, spec.Width * spec.Height);
							}
							else
							{
								auto parentPath = path.parent_path();
								parentPath /= aiRoughnessTexPath.C_Str();
								HNB_CORE_INFO("    Roughness map path = {0}", parentPath.string());
								buffer = TextureSerializer::ToBufferFromFile(parentPath, spec);
							}
							AssetHandle textureHandle = AssetManager::CreateMemoryOnlyAsset<Texture2D>(spec, buffer);
							material->SetMetallicRoughnessTex(textureHandle);
						}
					}
				}
			}
			HNB_CORE_INFO("------------------------");
		}

		meshSource->Invalidate();
		return meshSource;
	}
}