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
	spec.WorkingDirectory = "../Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}