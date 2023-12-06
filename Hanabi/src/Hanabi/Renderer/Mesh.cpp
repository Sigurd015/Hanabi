#include "hnbpch.h"
#include "Mesh.h"

namespace Hanabi
{
	MeshSource::MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices) :m_Vertices(vertices), m_Indices(indices)
	{
		Invalidate();
	}

	void MeshSource::Invalidate()
	{
		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));

		m_VertexBuffer->SetLayout({
		  { ShaderDataType::Float3, "a_Position" },
		  { ShaderDataType::Float3, "a_Normal" },
		  { ShaderDataType::Float3, "a_Tangent" },
		  { ShaderDataType::Float3, "a_Bitangent" },
		  { ShaderDataType::Float2, "a_TexCoord" },
			});
	}

	Mesh::Mesh(Ref<MeshSource> source) : m_MeshSource(source) {}
}