#include <Hanabi.h>
#include "EditorLayer.h"
#include "imgui.h"
#include "Engine/EntryPoint.h"

namespace Hanabi
{
	class HazelEditor : public Application
	{
	public:
		HazelEditor(): Application("Hazel Editor")
		{
			PushLayer(new EditorLayer());
		}

		~HazelEditor()
		{}
	};

}

Hanabi::Application* CreateApplication()
{
	return new Hanabi::HazelEditor();
}