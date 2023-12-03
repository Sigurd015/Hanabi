#pragma once
#include "Hanabi.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ViewportPanel.h"

namespace Hanabi
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		void OnScenePlay();
		void OnSceneStop();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnWindowDrop(WindowDropEvent& e);
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
		void NewScene();
		void OpenScene(AssetHandle handle);
		void SaveScene();
		void SaveSceneAs();

		Ref<Scene> m_EditorScene;
		Ref<Scene> m_ActiveScene;
	};
}