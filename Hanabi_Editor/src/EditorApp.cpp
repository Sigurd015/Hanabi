#include <Hanabi.h>
#include "EditorLayer.h"
#include "imgui.h"
#include "Engine/EntryPoint.h"

namespace Hanabi
{
	class HanabiEditor : public Application
	{
	public:
		HanabiEditor(): Application("Hanabi Editor")
		{
			PushLayer(new EditorLayer());
		}

		~HanabiEditor()
		{}
	};

}

Hanabi::Application* CreateApplication()
{
	return new Hanabi::HanabiEditor();
}