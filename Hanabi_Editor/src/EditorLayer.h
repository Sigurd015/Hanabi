#pragma once
#include "Hanabi.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Framebuffer.h"
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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();
		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
		void UI_MenuBar();
		void UI_StatesPanel();
		void UI_Viewport();

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		bool m_PrimaryCamera = true;
		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;
		Entity m_HoveredEntity;
		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};
}