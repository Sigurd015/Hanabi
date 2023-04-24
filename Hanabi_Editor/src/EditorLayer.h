#pragma once
#include "Hanabi.h"
#include "Engine/Renderer/Framebuffer.h"

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
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		// Temp
		Entity m_MainCamera;
		Entity m_TestCamera;
		bool m_PrimaryCamera = true;
		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Ref<Framebuffer> m_Framebuffer;
	};
}