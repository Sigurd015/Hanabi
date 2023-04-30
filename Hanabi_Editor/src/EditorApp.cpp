#include <Hanabi.h>
#include "EditorLayer.h"
#include "imgui.h"
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

	return new HanabiEditor(spec);
}