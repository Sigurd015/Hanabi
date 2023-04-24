#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Scene/Entity.h"
#include <entt.hpp>

namespace Hanabi
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();
		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		Entity CreateEntity(const std::string& name = std::string());	
		void DestroyEntity(Entity entity);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		entt::registry m_Registry;		
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}