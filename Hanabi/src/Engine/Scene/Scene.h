#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"
#include <entt.hpp>

class b2World;
namespace Hanabi
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();
		static Ref<Scene> Copy(Ref<Scene> other);
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnViewportResize(uint32_t width, uint32_t height);
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();
		void DuplicateEntity(Entity entity);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		b2World* m_PhysicsWorld = nullptr;
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};
}