#include <Hanabi.h>
#include "Engine/EntryPoint.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/KeyEvent.h"

class ExampleLayer :public Hanabi::Layer
{
public:
	ExampleLayer() :Layer("ExampleLayer") {}
	~ExampleLayer() {}
	void OnAttach() override {}
	void OnDetach() override {}
	void OnUpdate(Hanabi::Timestep ts)override
	{
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
	void OnImGuiRender()override {}

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