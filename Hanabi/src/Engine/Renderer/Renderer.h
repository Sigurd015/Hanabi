#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "RenderCommandQueue.h"

namespace Hanabi
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void SetClearColor(const glm::vec4& color);
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginRender();
		static void EndRender();
		static void BeginRenderPass(const Ref<RenderPass> renderPass);
		static void EndRenderPass(const Ref<RenderPass> renderPass);

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr)
			{
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
			};
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void SubmitStaticMesh(const Ref<Mesh> mesh, const Ref<Pipeline> pipeline);
		static void DrawIndexed(const Ref<VertexBuffer> vertexBuffer, const Ref<IndexBuffer> indexBuffer,
			const Ref<Pipeline> pipeline, uint32_t indexCount = 0);
		static void DrawLines(const Ref<VertexBuffer> vertexBuffer, const Ref<Pipeline> pipeline, uint32_t vertexCount);
		static void SetLineWidth(float width);
		static void WaitAndRender();
	private:
		static RenderCommandQueue& GetRenderCommandQueue();
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static Scope<SceneData> s_SceneData;
	};
}
