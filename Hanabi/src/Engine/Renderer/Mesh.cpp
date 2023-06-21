#include "hnbpch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Hanabi
{
	MeshSource::MeshSource(const std::string& filename)
	{
		Assimp::Importer importer;
		auto pAssimpScene = importer.ReadFile("assets/models/" + filename,
			aiProcess_ConvertToLeftHanded |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords
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
				m_Vertices.push_back({
					{vertex.x, vertex.y, vertex.z},
					{normal.x, normal.y, normal.z},
					{texCoord.x, texCoord.y}
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

	Mesh::Mesh(Ref<MeshSource> source) : m_Vertices(source->GetVertices()), m_Indices(source->GetIndices())
	{
		m_VertexBuffer = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
		m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		//TODO: Make layout dynamic
		VertexBufferLayout layout = {
		  { ShaderDataType::Float3, "a_Position" },
		  { ShaderDataType::Float3, "a_Normal" },
		  { ShaderDataType::Float2, "a_TexCoord" },
		};

		m_VertexBuffer->SetLayout(layout);
	}

	void Mesh::Update(const glm::mat4& transform)
	{
		//TODO: Why can't update vertices at CPU side? or is this function not correct?
		//HNB_CORE_ASSERT(m_Vertices.size() != 0, "Vertices size cannot be 0");
		//for (auto& vertex : m_Vertices)
		//{
		//	vertex.Position = transform * glm::vec4(vertex.Position, 1.0f);
		//	vertex.Normal = transform * glm::vec4(vertex.Normal, 1.0f);
		//}
		//m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	}
}