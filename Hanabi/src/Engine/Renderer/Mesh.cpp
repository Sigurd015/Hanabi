#include "hnbpch.h"
#include "Mesh.h"

namespace Hanabi
{
	StaticMesh::StaticMesh(const std::string& filename)
	{}

	StaticMesh::StaticMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices)
	{
		m_VertexBuffer = VertexBuffer::Create(&vertices[0], vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(&(indices[0].V1), indices.size() * sizeof(Index));
	}
}