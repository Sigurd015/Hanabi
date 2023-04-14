#include <Hanabi.h>

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
};

class Sandbox :public Hanabi::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Hanabi::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}