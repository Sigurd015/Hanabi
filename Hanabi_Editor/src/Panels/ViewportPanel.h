#pragma once
#include "Hanabi.h"
#include "PanelManager.h"

namespace Hanabi
{
	class ViewPortPanel : public Panel
	{
	public:
		ViewPortPanel();

		void SetContext(const Ref<Scene>& scene);

		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		void RegisterOnScenePlayCallback(const std::function<void()>& callback) { m_OnScenePlayCallback = callback; }
		void RegisterOnSceneStopCallback(const std::function<void()>& callback) { m_OnSceneStopCallback = callback; }
		void RegisterOnSceneOpenCallback(const std::function<void(AssetHandle)>& callback) { m_OnSceneOpenCallback = callback; }
	private:
		void DrawGizmos();

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void OnDuplicateEntity();

		Ref<Scene> m_Context;
		Ref<Image2D> m_ViewPortImage;
		bool m_ViewportHovered = false;
		Entity m_HoveredEntity;
		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		enum class SceneState
		{
			Edit = 0, Play = 1,
		};
		SceneState m_SceneState = SceneState::Edit;

		std::function<void()> m_OnScenePlayCallback;
		std::function<void()> m_OnSceneStopCallback;
		std::function<void(AssetHandle)> m_OnSceneOpenCallback;
	};
}
