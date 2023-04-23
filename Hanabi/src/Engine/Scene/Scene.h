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
		Entity CreateEntity(const std::string& name = std::string());
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}