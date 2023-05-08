#include <Hanabi.h>
#include "Engine/EntryPoint.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/KeyEvent.h"

#include <imgui.h>

struct Vertex
{
	glm::vec3 Pos;
};

Vertex vertices[] =
{
	{ glm::vec3(-1.0f, -1.0f, -1.0f)},
	{ glm::vec3(-1.0f, 1.0f, -1.0f)},
	{ glm::vec3(1.0f, 1.0f, -1.0f) },
	{ glm::vec3(1.0f, -1.0f, -1.0f)},
	{ glm::vec3(-1.0f, -1.0f, 1.0f)},
	{ glm::vec3(-1.0f, 1.0f, 1.0f)},
	{ glm::vec3(1.0f, 1.0f, 1.0f) },
	{ glm::vec3(1.0f, -1.0f, 1.0f)}
};

uint32_t indices[] =
{
  0, 1, 2,
  2, 3, 0,
  4, 5, 1,
  1, 0, 4,
  1, 5, 6,
  6, 2, 1,
  7, 6, 5,
  5, 4, 7,
  3, 2, 6,
  6, 7, 3,
  4, 0, 3,
  3, 7, 4
};

struct ConstantBuffer
{
	glm::mat4 World;
	glm::mat4 View;
	glm::mat4 Proj;
	glm::vec4 Color;
};


class ExampleLayer :public Hanabi::Layer
{
private:
	Hanabi::Ref<Hanabi::VertexArray> m_VertexArray;
	Hanabi::Ref<Hanabi::VertexBuffer>	m_VertexBuffer;
	Hanabi::Ref<Hanabi::IndexBuffer>	m_IndexBuffer;
	Hanabi::Ref<Hanabi::UniformBuffer> m_UniformBuffer;
	Hanabi::Ref<Hanabi::Texture2D> m_Texture;
	Hanabi::Ref<Hanabi::Shader>	m_Shader;
	ConstantBuffer m_ConstantBuffer = {};
	float Phi, Theta, Scale, Tx, Ty;
	glm::vec4 color = { 0.3f,0.3f,0.3f,1.0f };
public:
	ExampleLayer() :Layer("ExampleLayer") {}
	~ExampleLayer() {}
	void OnAttach() override
	{
		m_VertexArray = Hanabi::VertexArray::Create();
		m_VertexBuffer = Hanabi::VertexBuffer::Create(60 * sizeof(Vertex));
		m_VertexBuffer->SetLayout({
				{ Hanabi::ShaderDataType::Float3, "Position" },
				/*			{ Hanabi::ShaderDataType::Float4, "Color"    },*/
							/*{ Hanabi::ShaderDataType::Float2, "TexCoord" },*/
			});
		m_Shader = Hanabi::Shader::Create("assets/shaders/dx11/TestShader.hlsl");
		m_VertexArray->AddVertexBuffer(m_VertexBuffer, m_Shader);
		m_IndexBuffer = Hanabi::IndexBuffer::Create(indices, (uint32_t)std::size(indices));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_VertexBuffer->SetData(vertices, sizeof(vertices));
		m_UniformBuffer = Hanabi::UniformBuffer::Create(sizeof(ConstantBuffer), 0);

		//m_Texture = Hanabi::Texture2D::Create("assets/textures/Checkerboard.png");
		/*	m_WhiteTexture = Hanabi::Texture2D::Create(1,1);
			uint32_t whiteTextureData = 0xffffffff;
			m_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));*/

		m_ConstantBuffer.World = glm::mat4(1.0f);    
		m_ConstantBuffer.View = glm::transpose(glm::lookAtLH(
			glm::vec3(0.0f, 0.0f, -5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)));
		m_ConstantBuffer.Proj = glm::transpose(glm::perspectiveFovLH(1.570796327f, 1920.0f, 1080.0f, 1.0f, 1000.0f));
		Tx = Ty = Phi = Theta = 0.0f;
		Scale = 1.0f;
		m_ConstantBuffer.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	void OnDetach() override {}
	void OnUpdate(Hanabi::Timestep ts)override
	{
		Hanabi::RenderCommand::SetClearColor(color);
		Hanabi::RenderCommand::Clear();

		m_ConstantBuffer.World = glm::transpose(glm::scale(glm::mat4(1.0f), glm::vec3(Scale)) *
			glm::rotate(glm::mat4(1.0f), Phi, glm::vec3(1.0f, 0, 0)) * glm::rotate(glm::mat4(1.0f), Theta, glm::vec3(0, 1.0f, 0)) *
			glm::translate(glm::mat4(1.0f), glm::vec3(Tx, Ty, 0.0f)));

		m_UniformBuffer->SetData(&m_ConstantBuffer, sizeof(ConstantBuffer));

		//m_Texture->Bind(1);

		Hanabi::Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));

		//HNB_INFO("Time Step:{0}", ts);
	}

	void OnEvent(Hanabi::Event& evnet) override
	{
		Hanabi::EventDispatcher dispatcher(evnet);
		//dispatcher.Dispatch<Hanabi::KeyPressedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnKeyPressed));
		//dispatcher.Dispatch<Hanabi::KeyReleasedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnKeyReleased));
		//dispatcher.Dispatch<Hanabi::KeyTypedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnKeyTyped));
		//dispatcher.Dispatch<Hanabi::MouseMovedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnMouseMove));
		//dispatcher.Dispatch<Hanabi::MouseButtonPressedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnMouseButtonDown));
		//dispatcher.Dispatch<Hanabi::MouseButtonReleasedEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnMouseButtonUp));
		//dispatcher.Dispatch<Hanabi::MouseScrolledEvent>(HNB_BIND_EVENT_FN(ExampleLayer::OnMouseScrolled));
	}
	void OnImGuiRender()override
	{
		if (ImGui::Begin("Tools"))
		{
			if (ImGui::Button("Reset Params"))
			{
				Tx = Ty = Phi = Theta = 0.0f;
				Scale = 1.0f;
			}
			ImGui::SliderFloat("Scale", &Scale, 0.2f, 2.0f);

			ImGui::Text("Phi: %.2f degrees", glm::degrees(Phi));
			ImGui::SliderFloat("##1", &Phi, -3.1415926f, 3.1415926f, "");
			ImGui::Text("Theta: %.2f degrees", glm::degrees(Theta));
			ImGui::SliderFloat("##2", &Theta, -3.1415926f, 3.1415926f, "");
			ImGui::ColorEdit4("Color:", &m_ConstantBuffer.Color.x);
		}
		ImGui::End();
	}

	bool OnMouseMove(Hanabi::MouseMovedEvent& event)
	{
		HNB_INFO("Mouse Pos:X({0}) Y({1})", event.GetX(), event.GetY());
		return false;
	}

	bool OnMouseButtonDown(Hanabi::MouseButtonPressedEvent& event)
	{
		HNB_INFO("MouseButton:{0} Down", event.ToString());
		return false;
	}

	bool OnMouseButtonUp(Hanabi::MouseButtonReleasedEvent& event)
	{
		HNB_INFO("MouseButton:{0} Up", event.ToString());
		return false;
	}

	bool OnMouseScrolled(Hanabi::MouseScrolledEvent& event)
	{
		HNB_INFO("MouseScrolled:X({0}) Y({1})", event.GetXOffset(), event.GetYOffset());
		return false;
	}

	bool OnKeyPressed(Hanabi::KeyPressedEvent& event)
	{
		if (event.IsRepeat())
		{
			HNB_INFO("Key:{0} Repeat", event.GetKeyCode());
			return false;
		}
		HNB_INFO("Key:{0} Down", event.GetKeyCode());
		return false;
	}

	bool OnKeyReleased(Hanabi::KeyReleasedEvent& event)
	{
		HNB_INFO("Key:{0} Up", event.GetKeyCode());
		return false;
	}

	bool OnKeyTyped(Hanabi::KeyTypedEvent& event)
	{
		HNB_INFO("Key:{0} Typed", event.GetKeyCode());
		return false;
	}
};

class Sandbox :public Hanabi::Application
{
public:
	Sandbox(const Hanabi::ApplicationSpecification& specification) : Hanabi::Application(specification)
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* Hanabi::CreateApplication(Hanabi::ApplicationCommandLineArgs args)
{
	Hanabi::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Hanabi_Editor";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}