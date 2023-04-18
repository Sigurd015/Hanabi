#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/ApplicationEvent.h"

namespace Hanabi
{
	class CameraController
	{
	public:
		virtual ~CameraController() = default;
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event& e) = 0;
		virtual Camera& GetCamera() = 0;
		virtual const Camera& GetCamera() const = 0;
	};

	class CameraController2D :public CameraController
	{
	public:
		CameraController2D(float aspectRatio, bool rotation = false);
		~CameraController2D() = default;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		Camera& GetCamera() override { return m_Camera; }
		const Camera& GetCamera() const override { return m_Camera; }
		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);	
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		bool m_Rotation;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 35.0f, m_CameraRotationSpeed = 180.0f;
		Camera2D m_Camera;
	};
}
