#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "EditorCamera.h"

#define MAX_POINT_LIGHT 2
#define MAX_SPOT_LIGHT 2

namespace Hanabi
{
	struct DirectionalLight
	{
		glm::vec3 Color = { 1.0f,1.0f,1.0f };
		float Intensity = 1.0f;
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };

		// Padding
		float padding;
	};

	struct PointLight
	{
		glm::vec3 Color = { 1.0f,1.0f,1.0f };
		float Intensity = 1.0f;
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Constant = 1.0f;
		float Linear = 0.045f;
		float Exp = 0.0075f;

		// Padding
		float padding[2];
	};

	struct SpotLight
	{
		glm::vec3 Color = { 1.0f,1.0f,1.0f };
		float Intensity = 1.0f;
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Cutoff = 0.0f;
		glm::vec3 Direction = { 0.0f, 0.0f, 1.0f };
		float Constant = 1.0f;
		float Linear = 0.045f;
		float Exp = 0.0075f;

		// Padding
		float padding[2];
	};

	struct Environment
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;

		DirectionalLight DirLight;

		uint32_t PointLightCount = 0;
		PointLight PointLights[MAX_POINT_LIGHT];

		uint32_t SpotLightCount = 0;
		SpotLight SpotLights[MAX_SPOT_LIGHT];
	};

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Environment& environment);
		static void EndScene();

		static Ref<Framebuffer> GetFinalResult();

		static void SubmitStaticMesh(const Ref<Mesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID = -1);
	};
}