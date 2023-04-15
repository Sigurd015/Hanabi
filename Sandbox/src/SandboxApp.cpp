#include <Hanabi.h>
#include "imgui.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"
#include <Engine/Renderer/OpenGL/OpenGLBuffer.h>

class ExampleLayer : public Hanabi::Layer
{
public:

	std::shared_ptr<Hanabi::Shader> m_Shader;
	std::shared_ptr<Hanabi::VertexArray> m_VertexArray;

	ExampleLayer() : Layer("Example")
	{
		float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		unsigned int indices[3] = { 0, 1, 2 };
		//VertexArray
		m_VertexArray.reset(Hanabi::VertexArray::Create());
		m_VertexArray->Bind();
		//VertexBuffer
		std::shared_ptr<Hanabi::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hanabi::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout({
			{ Hanabi::ShaderDataType::Float3, "a_Position" },
			{ Hanabi::ShaderDataType::Float4, "a_Color" } });
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		//IndexBuffer
		std::shared_ptr<Hanabi::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hanabi::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;
			
            out vec4 v_Color;
			void main()
			{
                v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
            in vec4 v_Color;
			void main()
			{
				color = v_Color;
			}
		)";
		m_Shader.reset(new Hanabi::Shader(vertexSrc, fragmentSrc));
	}

	void OnUpdate() override
	{
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_TAB))
			HNB_TRACE("Tab key is pressed (poll)!");

		Hanabi::RenderCommand::SetClearColor({ 0.3f, 0.3f, 0.3f, 1 });
		Hanabi::RenderCommand::Clear();

		Hanabi::Renderer::BeginScene();

		m_Shader->Bind();
		Hanabi::Renderer::Submit(m_VertexArray);

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
		/*	ImGui::Begin("Test");
			ImGui::Text("Hello World");
			ImGui::End();*/
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