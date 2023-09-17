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
		void OpenScene(AssetHandle handle);
		void SaveScene();
		void SaveSceneAs();
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
		Ref<Scene> m_ActiveScene;

		Ref<Image2D> m_ViewportImage;
		// --- Deferred Rendering Debug ---
		Ref<Image2D> m_FinalResultImage;
		Ref<Image2D> m_GBufferDiffuseImage;
		Ref<Image2D> m_GBufferSpecularImage;
		Ref<Image2D> m_GBufferNormalImage;
		Ref<Image2D> m_GBufferPositionImage;
		Ref<Image2D> m_LightingResultImage;
		Ref<Image2D> m_ShadowMappingResultImage;
		enum ViewportDebugMode
		{
			None = 0,
			GBufferDiffuse = 1,
			GBufferSpecular = 2,
			GBufferNormal = 3,
			GBufferPosition = 4,
			LightingResult = 5,
			ShadowMappingResult = 6,
			Count = 7,
		};
		ViewportDebugMode m_ViewportDebugMode = None;

		bool m_ShowPhysicsColliders = false;
		bool m_EnableVsyn = false;

		Entity m_HoveredEntity;

		//Debug
		bool m_ImGuiDebugWndDraw = false;

	};
}