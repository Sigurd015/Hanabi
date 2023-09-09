#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "EditorCamera.h"
#include "Engine/Scene/Scene.h"

namespace Hanabi
{
	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Ref<Environment> environment);
		static void EndScene();

		static Ref<RenderPass> GetFinalRenderPass();

		static void SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<MaterialAsset>& material);
		static void SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh);
		static void SubmitStaticMesh(const glm::mat4& transform, MeshComponent& meshComponent, AssetHandle materialAssetHandle = 0);

	private:
		static void ExecuteDrawCommands(const Ref<Pipeline>& pipeline, const Ref<Material>& material = nullptr);
		static void ShadowPass();
		static void GeometryPass();
		static void SkyboxPass();
		static void CompositePass();
	};
}