#include "hnbpch.h"
#include "Mesh.h"

namespace Hanabi
{
	StaticMesh::StaticMesh(const std::string& filename)
	{}

	StaticMesh::StaticMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices)
	{
		m_VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(indices.data(), indices.size() * sizeof(Index));

		//TODO: Make layout dynamic
		VertexBufferLayout layout = {
	      { ShaderDataType::Float3, "a_Position" },
	      { ShaderDataType::Float3, "a_Normal" },
	      { ShaderDataType::Float2, "a_TexCoord" },
	      { ShaderDataType::Int,    "a_EntityID" }
		};

		m_VertexBuffer->SetLayout(layout);
	}
}