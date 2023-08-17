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
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 TexCoord;
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	class MeshSource :public Asset
	{
	public:
		MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices);

		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<Index>& GetIndices() const { return m_Indices; }

		static AssetType GetStaticType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;
	};

	class Mesh :public Asset
	{
	public:
		Mesh(Ref<MeshSource> source);
		~Mesh() = default;

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		const Ref<MeshSource>& GetMeshSource() const { return m_MeshSource; }

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		Ref<MeshSource> m_MeshSource;
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}