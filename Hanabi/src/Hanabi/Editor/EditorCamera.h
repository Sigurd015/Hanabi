#pragma once
#include "Hanabi/Renderer/Camera.h"
#include "Hanabi/Core/Timestep.h"
#include "Hanabi/Events/Event.h"
#include "Hanabi/Events/MouseEvent.h"

#include <utility>

namespace Hanabi
{
	enum class CameraMode
	{
		NONE, FLYCAM, ARCBALL
	};

	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
		void Init();

		void Focus(const glm::vec3& focusPoint);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		CameraMode GetCurrentMode() const { return m_CameraMode; }

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }

		void SetViewportSize(uint32_t width, uint32_t height);

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return GetProjection() * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
	private:
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
		float GetCameraSpeed() const;
	private:
		float m_FOV, m_AspectRatio, m_NearClip, m_FarClip;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Direction;
		glm::vec3 m_Position = { 0.0f, 0.0f, 5.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		CameraMode m_CameraMode{ CameraMode::ARCBALL };

		float m_Distance = 10.0f;
		float m_NormalSpeed{ 0.002f };

		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_PitchDelta{}, m_YawDelta{};
		glm::vec3 m_PositionDelta{};
		glm::vec3 m_RightDirection{};

		float m_MinFocusDistance{ 100.0f };

		uint32_t m_ViewportWidth{ 1280 }, m_ViewportHeight{ 720 };

		constexpr static float MIN_SPEED{ 0.0005f }, MAX_SPEED{ 2.0f };
	};
}