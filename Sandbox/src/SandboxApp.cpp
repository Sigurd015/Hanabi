#include <Hanabi.h>
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include "../../Hanabi/vendor/GLFW/include/GLFW/glfw3.h"
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Hanabi::Layer
{
public:
	Hanabi::Ref<Hanabi::Shader> m_Shader;
	Hanabi::Ref<Hanabi::VertexArray> m_VertexArray;
	Hanabi::Camera2D m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;
	glm::vec3 m_Color = { 0.2f, 0.3f, 0.8f };

	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		float vertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		unsigned int indices[6] = { 0, 1, 2,0,2,3 };
		//VertexArray
		m_VertexArray.reset(Hanabi::VertexArray::Create());
		m_VertexArray->Bind();
		//VertexBuffer
		Hanabi::Ref<Hanabi::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hanabi::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout({
			{ Hanabi::ShaderDataType::Float3, "a_Position" } });
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		//IndexBuffer
		Hanabi::Ref<Hanabi::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hanabi::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection*u_Transform*vec4(a_Position, 1.0);	
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
            uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";
		m_Shader.reset(Hanabi::Shader::Create(vertexSrc, fragmentSrc));
	}

	void OnUpdate(Hanabi::Timestep ts) override
	{
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_A))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Hanabi::Input::IsKeyPressed(HNB_KEY_D))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_W))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Hanabi::Input::IsKeyPressed(HNB_KEY_S))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_Q))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_E))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hanabi::RenderCommand::SetClearColor({ 0.3f, 0.3f, 0.3f, 1 });
		Hanabi::RenderCommand::Clear();

		Hanabi::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_Shader->Bind();
		m_Shader->SetUniform("u_Color", m_Color);
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hanabi::Renderer::Submit(m_Shader, m_VertexArray, transform);
			}
		}

		Hanabi::Renderer::EndScene();
	}

	void OnEvent(Hanabi::Event& event) override
	{
		if (event.GetEventType() == Hanabi::EventType::KeyPressed)
		{
			Hanabi::KeyPressedEvent& e = (Hanabi::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HNB_KEY_TAB)
				HNB_TRACE("Tab key is pressed (event)!");
			HNB_TRACE("{0}[{1}]", (char)e.GetKeyCode(), e.GetKeyCode());
		}
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color",glm::value_ptr(m_Color));
		ImGui::End();
	}
};

class Sandbox :public Hanabi::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}