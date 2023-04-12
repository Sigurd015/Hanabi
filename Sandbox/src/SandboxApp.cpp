#include <Hanabi.h>

class ExampleLayer : public Hanabi::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{}
	void OnUpdate() override
	{
		HNB_INFO("ExampleLayer::Update");
	}
	void OnEvent(Hanabi::Event& event) override
	{
		HNB_TRACE("{0}", event);
	}
};

class Sandbox :public Hanabi::Application
{
public :
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