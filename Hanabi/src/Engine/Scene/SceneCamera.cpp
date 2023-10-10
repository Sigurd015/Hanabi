#include "hnbpch.h"
#include "Engine/Scene/SceneCamera.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Hanabi
{
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		HNB_CORE_ASSERT(width > 0 && height > 0);
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			SetPerspectiveProjectionMatrix(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float width = m_OrthographicSize * m_AspectRatio;
			float height = m_OrthographicSize;
			SetOrthoProjectionMatrix(width, height, m_OrthographicNear, m_OrthographicFar);
		}
	}
}