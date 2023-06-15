#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Pipeline.h"

#include <vector>

namespace Hanabi
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Texcoord;
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	class StaticMesh
	{
	public:
		StaticMesh(const std::string& filename);
		StaticMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices);
		~StaticMesh() = default;

		void SetMaterial(const Ref<Material>& material) { m_Material = material; }
		
		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<Material> GetMaterial() { return m_Material; }
	private:	
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Material> m_Material;
	};
}