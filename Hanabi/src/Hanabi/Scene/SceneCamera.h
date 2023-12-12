#pragma once
#include "Hanabi/Renderer/Camera.h"

namespace Hanabi
{
	class SceneCamera :public Camera
	{
	public:
		SceneCamera();
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }

		void SetNearClip(float nearClip) { m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveNear = nearClip : m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetNearClip() const { return m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveNear : m_OrthographicNear; }
		void SetFarClip(float farClip) { m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveFar = farClip : m_OrthographicFar = farClip; RecalculateProjection(); }
		float GetFarClip() const { return m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveFar : m_OrthographicFar; }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
	private:
		void RecalculateProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
		float m_AspectRatio = 0.0f;
	};
}