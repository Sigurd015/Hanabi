#pragma once
#include "Renderer2D.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "EditorCamera.h"

#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT 32

namespace Hanabi
{
	struct DirectionalLight
	{
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float Intensity = 0.0f;
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };

		// Padding
		float padding;
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float Radius;
		float Falloff;

		// Padding
		float padding[3];
	};

	struct SpotLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float AngleAttenuation;
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		float Range;
		float Angle;
		float Falloff;

		// Padding
		float padding[2];
	};

	struct Environment
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;

		DirectionalLight DirLight;

		std::vector<PointLight> PointLights;
		std::vector<SpotLight> SpotLights;
	};

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Environment& environment);
		static void EndScene();

		static Ref<RenderPass> GetFinalRenderPass();

		static void SubmitStaticMesh(const Ref<Mesh>& staticMesh, const Ref<Material>& material, const glm::mat4& transform, int entityID = -1);
	};
}