#include <Hanabi.h>
#include "Engine/EntryPoint.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/KeyEvent.h"

#include <imgui.h>

class ExampleLayer :public Hanabi::Layer
{
private:
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];
	Hanabi::Ref<Hanabi::Framebuffer> m_Framebuffer;
	Hanabi::EditorCamera m_EditorCamera;
public:
	ExampleLayer() :Layer("ExampleLayer") {}
	~ExampleLayer() {}
	void OnAttach() override
	{
		Hanabi::FramebufferSpecification fbSpec;
		fbSpec.Attachments = { Hanabi::FramebufferTextureFormat::RGBA8, Hanabi::FramebufferTextureFormat::RED_INTEGER ,Hanabi::FramebufferTextureFormat::Depth };
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		m_Framebuffer = Hanabi::Framebuffer::Create(fbSpec);
	}
	void OnDetach() override {}
	void OnUpdate(Hanabi::Timestep ts)override
	{
		// Resize
		if (Hanabi::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_EditorCamera.OnUpdate(ts);

		m_Framebuffer->Bind();
		Hanabi::RenderCommand::SetClearColor({ 0.3f,0.3f,0.3f,1.0f });
		Hanabi::RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		Hanabi::Renderer2D::ResetStats();
		Hanabi::Renderer2D::BeginScene(m_EditorCamera);
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Hanabi::Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), { x * 2.5f,y * 2.5f,1.0f }), color);
			}
		}
		Hanabi::Renderer2D::EndScene();

		m_Framebuffer->Unbind();

		//Hanabi_INFO("Timestep:",ts);
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
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		ImGui::Begin("Settings");
		auto stats = Hanabi::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();

		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_Framebuffer->GetColorAttachment(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
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