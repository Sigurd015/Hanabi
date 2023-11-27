#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
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

		static Ref<RenderPass> GetFinalPass();
		//----- GBuffer Debugging -----
		static Ref<Image2D> GetGBufferAlbedo();
		static Ref<Image2D> GetGBufferMRE();
		static Ref<Image2D> GetGBufferNormal();
		static Ref<Image2D> GetGBufferPosition();

		static void SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<MaterialAsset>& material);
		static void SubmitStaticMesh(const glm::mat4& transform, const Ref<Mesh>& mesh);
		static void SubmitStaticMesh(const glm::mat4& transform, MeshComponent& meshComponent, AssetHandle materialAssetHandle = 0);

	private:
		static void ExecuteDrawCommands();

		static void DeferredGeoPass();
		static void DeferredLightPass();

		static void DirShadowMapPass();

		static void CompositePass();
		static void SkyboxPass();
	};
}