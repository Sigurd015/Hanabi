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

		static void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear = true);
		static void EndRenderPass(const Ref<RenderPass>& renderPass);

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

		//3D Objects
		static void SubmitStaticMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Pipeline>& pipeline, const glm::mat4& transform, uint32_t modelCBBingID = 0);

		//2D Objects
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			const Ref<Pipeline>& pipeline, uint32_t indexCount);
		static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount);

		static void WaitAndRender();

		static Ref<Shader> GetShader(const std::string& name);
		static Ref<Mesh> GetMesh(const std::string& name);
		static Ref<Texture2D> GetTexture(const std::string& name);
	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	};
}
