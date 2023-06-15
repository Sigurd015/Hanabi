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
		SceneRenderer();
		~SceneRenderer();

		void Init();
		void Shutdown();

		void BeginScene(const Camera& camera, const glm::mat4& transform);
		void BeginScene(const EditorCamera& camera);
		void EndScene();

		//3D
		void SubmitStaticMesh(const Ref<StaticMesh>& staticMesh,const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);

		Ref<Renderer2D> GetRenderer2D() { return m_Renderer2D; }
	private:
		Ref<Renderer2D> m_Renderer2D;
		Ref<Pipeline> m_DefaultPipeline;
		Ref<Shader> m_DefaultShader;
		Ref<Material> m_DefaultMaterial;

		struct SceneData
		{
			glm::mat4 Model;
			glm::mat4 ViewProjection;
		};

		SceneData SceneBuffer;
		Ref<ConstantBuffer> SceneConstantBuffer;
	};
}
