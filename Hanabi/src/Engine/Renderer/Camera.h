#pragma once
#include <glm/glm.hpp>

namespace Hanabi
{
	class Camera
	{
	public:
		virtual ~Camera() = default;
		virtual const glm::vec3& GetPosition() const = 0;
		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual float GetRotation() const = 0;
		virtual	void SetRotation(float rotation) = 0;
		virtual	const glm::mat4& GetProjectionMatrix() const = 0;
		virtual	const glm::mat4& GetViewMatrix() const = 0;
		virtual	const glm::mat4& GetViewProjectionMatrix() const = 0;
	};

	class Camera2D :public Camera
	{
	public:
		Camera2D(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);
		const glm::vec3& GetPosition() const override { return m_Position; }
		void SetPosition(const glm::vec3& position) override { m_Position = position; RecalculateViewMatrix(); }
		float GetRotation() const override { return m_Rotation; }
		void SetRotation(float rotation)override { m_Rotation = rotation; RecalculateViewMatrix(); }
		const glm::mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const override { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};
}