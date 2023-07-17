#include "Hanabi.h"
#include "Engine/EntryPoint.h"

#include <imgui.h>

class Sandbox :public Hanabi::Application
{
public:
	Sandbox(const Hanabi::ApplicationSpecification& specification) : Hanabi::Application(specification)
	{
		
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
	spec.EnableImGui = true;
	spec.EnableScripting = false;
	spec.RendererConfig.APIType = RendererAPIType::DX11;

	return new Sandbox(spec);
}