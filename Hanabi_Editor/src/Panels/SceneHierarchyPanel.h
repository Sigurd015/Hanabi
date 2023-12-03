#pragma once
#include "Hanabi.h"
#include "PanelManager.h"

namespace Hanabi
{
	class SceneHierarchyPanel : public Panel
	{
	public:
		SceneHierarchyPanel() = default;
		void SetContext(const Ref<Scene>& scene);
		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
	};
}