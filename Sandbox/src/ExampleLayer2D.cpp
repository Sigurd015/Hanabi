#include "ExampleLayer2D.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

void ExampleLayer2D::OnAttach()
{
	m_CheckerboardTexture = Hanabi::Texture2D::Create("assets/textures/Checkerboard.png");
	m_BaseTexture = Hanabi::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
	m_SubTexture = Hanabi::SubTexture2D::CreateFromCoords(m_BaseTexture, glm::vec2(1, 1), glm::vec2(128, 128), glm::vec2(1, 2));
}

void ExampleLayer2D::OnDetach()
{}

void ExampleLayer2D::OnUpdate(Hanabi::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Hanabi::Renderer2D::ResetStats();
	Hanabi::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hanabi::RenderCommand::Clear();

	static float rotation = 0.0f;
	rotation += ts * 45.0f;

	Hanabi::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(1 * rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 1.0f }),
		m_SquareColor);
	Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), { 1.5f, 1.5f, 1.0f }),
		m_SquareColor);
	Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(1.3f * rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f }),
		m_SquareColor);
	Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-1 * rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f }),
		m_SquareColor);

	for (float y = -5.0f; y < 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x < 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(-1 * rotation), { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { 0.45f, 0.45f, 1.0f }),
				color);
		}
	}

	Hanabi::Renderer2D::EndScene();
}

void ExampleLayer2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	auto stats = Hanabi::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void ExampleLayer2D::OnEvent(Hanabi::Event& event)
{
	m_CameraController.OnEvent(event);
}