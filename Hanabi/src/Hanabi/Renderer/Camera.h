#pragma once

#include <glm/glm.hpp>

namespace Hanabi
{
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		void SetPerspectiveProjectionMatrix(const float radFov, const float aspect, const float nearP, const float farP)
		{
			m_Projection = glm::perspective(radFov, aspect, nearP, farP);
		}

		void SetOrthoProjectionMatrix(const float width, const float height, const float nearP, const float farP)
		{
			m_Projection = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
		}

		Camera(const glm::mat4& projection) : m_Projection(projection) {}
		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}