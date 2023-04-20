#include <Hanabi.h>
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/EntryPoint.h"
#include <Engine/Input/CameraController.h>
#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })
struct ProfileResult
{
	const char* Name;
	float Time;
};

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
	Hanabi::CameraController2D m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f,1.0f };
	std::vector<ProfileResult> m_ProfileResults;

	ExampleLayer2D() : Layer("ExampleLayer3D"), m_CameraController(1920.0f / 1080.0f)
	{}

	void ExampleLayer2D::OnAttach()
	{
		m_CheckerboardTexture = Hanabi::Texture2D::Create("assets/textures/Checkerboard.png");
	}

	void ExampleLayer2D::OnDetach()
	{}

	void ExampleLayer2D::OnUpdate(Hanabi::Timestep ts)
	{
		PROFILE_SCOPE("Sandbox2D::OnUpdate");

		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Hanabi::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hanabi::RenderCommand::Clear();

		Hanabi::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			PROFILE_SCOPE("Sandbox2D::DrawQuad");
			Hanabi::Renderer2D::DrawQuad({ -1.0f, 1.0f }, { 0.5f, 0.5f }, 95.0f, m_SquareColor);
			Hanabi::Renderer2D::DrawQuad({ 1.0f, 1.0f }, { 1.5f, 1.5f }, 0, m_SquareColor);
			Hanabi::Renderer2D::DrawQuad({ -1.0f, -1.0f }, { 1.0f, 1.0f }, 45.0f, m_SquareColor);
			Hanabi::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 1.0f, 1.0f }, 15.0f, m_SquareColor);
			Hanabi::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 10.0f, 10.0f }, 0, m_CheckerboardTexture);
		}
		Hanabi::Renderer2D::EndScene();
	}

	void ExampleLayer2D::OnImGuiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		for (auto& result : m_ProfileResults)
		{
			char label[50];
			strcpy(label, "%.3fms ");
			strcat(label, result.Name);
			ImGui::Text(label, result.Time);
		}
		m_ProfileResults.clear();

		ImGui::End();
	}

	void ExampleLayer2D::OnEvent(Hanabi::Event& e)
	{
		m_CameraController.OnEvent(e);
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