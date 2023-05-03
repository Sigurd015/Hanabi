#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/VertexArray.h"

namespace Hanabi
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
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
