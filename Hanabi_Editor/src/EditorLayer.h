#pragma once
#include "Hanabi.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

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
		void OnScenePause(bool pause);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowDrop(WindowDropEvent& e);
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();
		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
		void OnDuplicateEntity();

		//UI
		void UI_StatisticsPanel();
		void UI_ViewportPanel();

		enum class SceneState
		{
			Edit = 0, Play = 1,
		};
		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		bool m_PrimaryCamera = true;
		bool m_ShowPhysicsColliders = false;
		bool m_EnableVsyn = false;
		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_ViewportFramebuffer;
		Entity m_HoveredEntity;
		// Editor resources
		Ref<Texture2D> m_PlayIcon, m_StopIcon, m_PauseIcon, m_StepIcon;

		//Debug
		bool m_ImGuiDebugWndDraw = false;
	};
}