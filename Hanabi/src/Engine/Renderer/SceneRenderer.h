#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "EditorCamera.h"

namespace Hanabi
{
	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();

		static void SubmitStaticMesh(const Ref<Mesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID = -1);
	};
}