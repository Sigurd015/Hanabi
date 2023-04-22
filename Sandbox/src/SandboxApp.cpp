#include <Hanabi.h>
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/EntryPoint.h"
#include <Engine/Input/CameraController.h>

class ExampleLayer3D : public Hanabi::Layer
{
public:
	Hanabi::ShaderLibrary m_ShaderLibrary;
	Hanabi::Ref<Hanabi::VertexArray> m_VertexArray;
	Hanabi::CameraController2D m_CameraController;

	glm::vec3 m_Color = { 0.2f, 0.3f, 0.8f };

	ExampleLayer3D() : Layer("ExampleLayer3D"), m_CameraController(1920.0f / 1080.0f)
	{}

	void OnAttach()override
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

	void OnUpdate(Hanabi::Timestep ts) override
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

	void OnEvent(Hanabi::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color", glm::value_ptr(m_Color));
		ImGui::End();
	}

	void OnDetach() override
	{}
};

class ExampleLayer2D : public Hanabi::Layer
{
public:
	Hanabi::Ref<Hanabi::Texture2D> m_CheckerboardTexture;
	Hanabi::Ref<Hanabi::Texture2D> m_BaseTexture;
	Hanabi::Ref<Hanabi::SubTexture2D> m_SubTexture;
	Hanabi::CameraController2D m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f,1.0f };

	ExampleLayer2D() : Layer("ExampleLayer3D"), m_CameraController(1920.0f / 1080.0f)
	{}

	void ExampleLayer2D::OnAttach()
	{
		m_CheckerboardTexture = Hanabi::Texture2D::Create("assets/textures/Checkerboard.png");
		m_BaseTexture = Hanabi::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
		m_SubTexture = Hanabi::SubTexture2D::CreateFromCoords(m_BaseTexture, glm::vec2(7,6), glm::vec2(128, 128));
	}

	void ExampleLayer2D::OnDetach()
	{}

	void ExampleLayer2D::OnUpdate(Hanabi::Timestep ts)
	{
		HNB_PROFILE_FUNCTION();
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Hanabi::Renderer2D::ResetStats();
		Hanabi::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hanabi::RenderCommand::Clear();

		static float rotation = 0.0f;
		rotation += ts * 45.0f;

		Hanabi::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			HNB_PROFILE_SCOPE("Renderer Draw");

			//Hanabi::Renderer2D::DrawRotatedQuad({ -1.0f, 1.0f }, { 0.5f, 0.5f }, glm::radians(1 * rotation), m_SquareColor);
			//Hanabi::Renderer2D::DrawQuad({ 1.0f, 1.0f }, { 1.5f, 1.5f }, m_SquareColor);
			//Hanabi::Renderer2D::DrawRotatedQuad({ -1.0f, -1.0f }, { 1.0f, 1.0f }, glm::radians(1.3f * rotation), m_SquareColor);
			//Hanabi::Renderer2D::DrawRotatedQuad({ 1.0f, -1.0f }, { 1.0f, 1.0f }, glm::radians(-1 * rotation), m_SquareColor);
			//Hanabi::Renderer2D::DrawQuad({ 0.0f, 0.0f ,-0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);

			//Hanabi::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_SquareColor);

			Hanabi::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_SubTexture);
			/*for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
					Hanabi::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
				}
			}*/
		}
		Hanabi::Renderer2D::EndScene();
	}

	void ExampleLayer2D::OnImGuiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		auto stats = Hanabi::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();
	}

	void ExampleLayer2D::OnEvent(Hanabi::Event& event)
	{
		m_CameraController.OnEvent(event);

		if (event.GetEventType() == Hanabi::EventType::KeyPressed)
		{
			Hanabi::KeyPressedEvent& e = (Hanabi::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HNB_KEY_TAB)
				HNB_TRACE("Tab key is pressed (event)!");
			HNB_TRACE("{0}[{1}]", (char)e.GetKeyCode(), e.GetKeyCode());
		}
	}
};

class Sandbox :public Hanabi::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer3D());
		PushLayer(new ExampleLayer2D());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}