#pragma once
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Shader.h"

namespace Hanabi
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(Camera& camera);
		static void EndScene();
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform);
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static Scope<SceneData> s_SceneData;
	};
}
