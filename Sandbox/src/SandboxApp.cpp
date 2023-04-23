#include <Hanabi.h>
#include "Engine/EntryPoint.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"

class ExampleLayer :public Hanabi::Layer
{
public:
	ExampleLayer() :Layer("ExampleLayer") {}
	~ExampleLayer() {}
	void OnAttach() override {}
	void OnDetach() override {}
	void OnUpdate(Hanabi::Timestep ts)override {}
	void OnEvent(Hanabi::Event& evnet) override {}
	void OnImGuiRender()override {}
};

class Sandbox :public Hanabi::Application
{
public:
	Sandbox() :Hanabi::Application("SandBox")
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