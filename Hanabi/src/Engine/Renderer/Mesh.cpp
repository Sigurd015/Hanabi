#include "hnbpch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Hanabi
{
	MeshSource::MeshSource(const std::string& path) :m_Path(path)
	{
		Assimp::Importer importer;
		auto pAssimpScene = importer.ReadFile(path,
			aiProcess_ConvertToLeftHanded |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_CalcTangentSpace
		);

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
				m_Vertices.push_back({
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
				m_Indices.push_back({ face.mIndices[0],face.mIndices[1],face.mIndices[2] });
			}

			importer.FreeScene();
		}
	}

	MeshSource::MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices) :m_Vertices(vertices), m_Indices(indices)
	{}

	Mesh::Mesh(Ref<MeshSource> source) : m_Vertices(source->GetVertices()), m_Indices(source->GetIndices()), m_MeshSource(source)
	{
		m_VertexBuffer = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
		m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		//TODO: Make layout dynamic
		VertexBufferLayout layout = {
		  { ShaderDataType::Float3, "a_Position" },
		  { ShaderDataType::Float3, "a_Normal" },
		  { ShaderDataType::Float3, "a_Tangent" },
		  { ShaderDataType::Float3, "a_Bitangent" },
		  { ShaderDataType::Float2, "a_TexCoord" },
		};

		m_VertexBuffer->SetLayout(layout);
	}
}