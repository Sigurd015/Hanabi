#include "hnbpch.h"
#include "Mesh.h"

namespace Hanabi
{
	Mesh::Mesh(const std::string& filename)
	{}

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Index>& indices)
		: m_Vertices(vertices), m_Indices(indices)
	{
		m_VertexBuffer = VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(&(m_Indices[0].V1), m_Indices.size() * sizeof(Index));
	}
}