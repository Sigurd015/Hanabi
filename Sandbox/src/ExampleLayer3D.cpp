#include "ExampleLayer3D.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

void ExampleLayer3D::OnAttach()
{
	float vertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	uint32_t indices[6] = { 0, 1, 2, 0, 2, 3 };
	//VertexArray
	m_VertexArray = Hanabi::VertexArray::Create();
	//VertexBuffer
	Hanabi::Ref<Hanabi::VertexBuffer> vertexBuffer = Hanabi::VertexBuffer::Create(vertices, sizeof(vertices));
	vertexBuffer->SetLayout({
		{ Hanabi::ShaderDataType::Float3, "a_Position" } });
	m_VertexArray->AddVertexBuffer(vertexBuffer);
	//IndexBuffer
	m_VertexArray->SetIndexBuffer(Hanabi::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

	m_ShaderLibrary.Load("assets/shaders/Color.glsl");
}

void ExampleLayer3D::OnUpdate(Hanabi::Timestep ts) 
{
	m_CameraController.OnUpdate(ts);

	Hanabi::RenderCommand::SetClearColor({ 0.3f, 0.3f, 0.3f, 1 });
	Hanabi::RenderCommand::Clear();

	Hanabi::Renderer::BeginScene(m_CameraController.GetCamera());

	static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	m_ShaderLibrary.Get("Color")->Bind();
	m_ShaderLibrary.Get("Color")->SetUniform("u_Color", m_Color);
	m_VertexArray->Bind();
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Hanabi::Renderer::Submit(m_ShaderLibrary.Get("Color"), m_VertexArray, transform);
		}
	}

	Hanabi::Renderer::EndScene();
}

void ExampleLayer3D::OnEvent(Hanabi::Event& event) 
{
	m_CameraController.OnEvent(event);
}

 void ExampleLayer3D::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void ExampleLayer3D::OnDetach()
{}

