#include "Hanabi.h"
#include "EditorLayer.h"
#include "Engine/EntryPoint.h"

namespace Hanabi
{
	class HanabiEditor : public Application
	{
	public:
		HanabiEditor(const Hanabi::ApplicationSpecification& specification) : Hanabi::Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~HanabiEditor()
		{}
	};
}

Hanabi::Application* Hanabi::CreateApplication(ApplicationCommandLineArgs args)
{
	Hanabi::ApplicationSpecification spec;
	spec.Name = "Hanabi Editor";
	spec.WorkingDirectory = "../Hanabi_Editor";
	spec.CommandLineArgs = args;
	spec.EnableImGui = true;	
	spec.EnableScripting = true;
	spec.ScriptConfig.CoreAssemblyPath = "resources/scripts/Hanabi_ScriptCore.dll";

	return new HanabiEditor(spec);
}