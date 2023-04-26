#pragma once
#include "Hanabi.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Scene/Panel/SceneHierarchyPanel.h"

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
		bool OnKeyPressed(KeyPressedEvent& e);
		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		SceneHierarchyPanel m_SceneHierarchyPanel;
		bool m_PrimaryCamera = true;
		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;
	};
}