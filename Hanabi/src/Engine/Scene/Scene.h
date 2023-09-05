#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"
#include "Components.h"

#include <entt.hpp>

class b2World;
namespace Hanabi
{
	struct DirectionalLight
	{
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float Intensity = 0.0f;
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };

		LightComponent::ShadowType ShadowType = LightComponent::ShadowType::None;
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float Radius;
		float Falloff;

		LightComponent::ShadowType ShadowType = LightComponent::ShadowType::None;
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

		LightComponent::ShadowType ShadowType = LightComponent::ShadowType::None;
	};

	struct Environment
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;

		CameraComponent::ClearMethod ClearType = CameraComponent::ClearMethod::None;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		AssetHandle SkyboxAssetHandle = 0;

		AssetHandle SceneEnvironmentAssetHandle = 0;
		float SkyLightIntensity = 0.0f;

		DirectionalLight DirLight;

		std::vector<PointLight> PointLights;
		std::vector<SpotLight> SpotLights;
	};

	// Forward declaration
	class Entity;
	class Scene : public Asset
	{
	public:
		Scene();
		~Scene();
		static Ref<Scene> Copy(Ref<Scene> other);

		void OnUpdateRuntime(Timestep ts, Entity selectedEntity, bool enableOverlayRender = false);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity selectedEntity, bool enableOverlayRender = false);
		void OnRuntimeStart();
		void OnRuntimeStop();

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }
		void SetPaused(bool paused) { m_IsPaused = paused; }
		void Step(int frames = 1);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();
		Entity DuplicateEntity(Entity entity);
		Entity GetEntityByUUID(UUID uuid);
		Entity FindEntityByName(std::string_view name);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		static AssetType GetStaticType() { return AssetType::Scene; }
		virtual AssetType GetAssetType() const { return GetStaticType(); }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void RenderScene(Entity selectedEntity, bool enableOverlayRender);
		void OnOverlayRender(bool enable, Entity selectedEntity);
		void OnPhysics2DStart();
		void OnPhysics2DStop();

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		b2World* m_PhysicsWorld = nullptr;
		std::unordered_map<UUID, entt::entity> m_EntityMap;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		Ref<Environment> m_Environment;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};
}