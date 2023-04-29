#include <Hanabi.h>
#include "EditorLayer.h"
#include "imgui.h"
#include "Engine/EntryPoint.h"

namespace Hanabi
{
	class HanabiEditor : public Application
	{
	public:
		HanabiEditor(ApplicationCommandLineArgs args) : Application("Hanabi Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~HanabiEditor()
		{}
	};

}

Hanabi::Application* Hanabi::CreateApplication(ApplicationCommandLineArgs args)
{
	return new Hanabi::HanabiEditor(args);
}