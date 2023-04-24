#pragma once

#include "Engine/Base.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Hanabi
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);
		void SetContext(const Ref<Scene>& scene);
		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}