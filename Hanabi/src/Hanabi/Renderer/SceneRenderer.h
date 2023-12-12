#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Hanabi/Scene/Scene.h"

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
		static Ref<Image2D> GetDirShadowMap();

		static void SubmitStaticMesh(const glm::mat4& transform, MeshComponent& meshComponent, AssetHandle overrideMaterialHandle = 0);
		
		struct Statistics
		{
			uint32_t DrawCalls = 0;
		};

		static void ResetStats();
		static SceneRenderer::Statistics GetStats();
	private:
		static void ExecuteDrawCommands();

		static void DeferredGeoPass();
		static void DeferredLightPass();

		static void DirShadowMapPass();

		static void CompositePass();
		static void SkyboxPass();

	
		struct CascadeData
		{
			glm::mat4 ViewProj;
			glm::mat4 View;
			float SplitDepth;
		};
		static void CalculateCascades(CascadeData* cascades);
	};
}