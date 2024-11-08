#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Pipeline.h"
#include "Hanabi/Math/Math.h"

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

	struct Triangle
	{
		Vertex V0, V1, V2;

		Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
			: V0(v0), V1(v1), V2(v2) {}
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	class Submesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;

		AABB BoundingBox;

		glm::mat4 Transform{ 1.0f }; // World transform

		std::string MeshName;
	};

	class MeshSource :public Asset
	{
	public:
		MeshSource() = default;
		MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices);

		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<Index>& GetIndices() const { return m_Indices; }

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

		const AABB& GetBoundingBox() const { return m_BoundingBox; }

		std::vector<AssetHandle>& GetMaterials() { return m_Materials; }
		const std::vector<AssetHandle>& GetMaterials() const { return m_Materials; }

		const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }

		static AssetType GetStaticType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		void Invalidate();

		std::vector<Submesh> m_Submeshes;

		std::vector<AssetHandle> m_Materials;

		AABB m_BoundingBox;

		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		friend class MeshSourceSerializer;
	};

	class Mesh :public Asset
	{
	public:
		~Mesh() = default;
		Mesh(Ref<MeshSource> source);

		static Ref<Mesh> Create(Ref<MeshSource> source) { return CreateRef<Mesh>(source); }

		const Ref<MeshSource>& GetMeshSource() const { return m_MeshSource; }

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		Ref<MeshSource> m_MeshSource;
	};
}