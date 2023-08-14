#include "hnbpch.h"
#include "Mesh.h"

namespace Hanabi
{
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