#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/EditorCamera.h"
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
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnViewportResize(uint32_t width, uint32_t height);
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();
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