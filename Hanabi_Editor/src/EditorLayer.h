#pragma once
#include "Hanabi.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Scene/Panel/SceneHierarchyPanel.h"
#include "Engine/Scene/Panel/ContentBrowserPanel.h"

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
		void UI_MenuBar();
		void UI_StatesPanel();
		void UI_Viewport();
		bool OnKeyPressed(KeyPressedEvent& e);
		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		bool m_PrimaryCamera = true;
		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;
		Entity m_HoveredEntity;
	};
}