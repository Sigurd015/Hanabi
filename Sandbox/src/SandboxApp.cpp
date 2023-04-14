#include <Hanabi.h>
#include "imgui.h"

class ExampleLayer : public Hanabi::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{}

	void OnUpdate() override
	{
		if (Hanabi::Input::IsKeyPressed(HNB_KEY_TAB))
			HNB_TRACE("Tab key is pressed (poll)!");
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