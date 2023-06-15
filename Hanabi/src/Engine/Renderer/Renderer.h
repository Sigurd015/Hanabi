#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Mesh.h"

namespace Hanabi
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetClearColor(const glm::vec4& color);
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void ResetToSwapChain();

		static void BeginRenderPass(const Ref<RenderPass>& renderPass);
		static void EndRenderPass(const Ref<RenderPass>& renderPass);

		//3D Objects
		static void SubmitStaticMesh(const Ref<StaticMesh>& mesh, const Ref<Pipeline>& pipeline);

		//2D Objects
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			const Ref<Pipeline>& pipeline, uint32_t indexCount);
		static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount);

		static Ref<Texture2D> GetWhiteTexture();
		static Ref<StaticMesh> GetCubeMesh();
		static Ref<StaticMesh> GetSphereMesh();
		static Ref<StaticMesh> GetCapsuleMesh();
	};
}
