#include "hnbpch.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/KeyCodes.h"
#include "Engine/Input/MouseCodes.h"

#include <glfw/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hanabi
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
		Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::Focus(const glm::vec3& focusPoint)
	{
		m_FocalPoint = focusPoint;

		UpdateView();
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse = Input::GetMousePosition();
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}
		else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			float speed = m_MoveSpeed;
			if (Input::IsKeyPressed(Key::LeftControl))
				speed /= 2 - glm::log(m_MoveSpeed);
			if (Input::IsKeyPressed(Key::LeftShift))
				speed *= 2 - glm::log(m_MoveSpeed);
			speed = glm::clamp(speed, MIN_SPEED, MAX_SPEED);

			if (Input::IsKeyPressed(Key::W))
				m_Position += GetForwardDirection() * speed * 0.05f;
			if (Input::IsKeyPressed(Key::S))
				m_Position -= GetForwardDirection() * speed * 0.05f;
			if (Input::IsKeyPressed(Key::A))
				m_Position -= GetRightDirection() * speed * 0.05f;
			if (Input::IsKeyPressed(Key::D))
				m_Position += GetRightDirection() * speed * 0.05f;
			if (Input::IsKeyPressed(Key::Q))
				m_Position += GetUpDirection() * speed * 0.05f;
			if (Input::IsKeyPressed(Key::E))
				m_Position -= GetUpDirection() * speed * 0.05f;

			m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;

			const glm::vec2& mouse = Input::GetMousePosition();
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HNB_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::SetViewportSize(float width, float height)
	{
		if ((m_ViewportWidth == width && m_ViewportHeight == height) || (width <= 0 && height <= 0))
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;
		UpdateProjection();
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * m_RotationSpeed;
		m_Pitch += delta.y * m_RotationSpeed;
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
	}
}