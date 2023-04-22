#include <Hanabi.h>
#include "EditorLayer.h"
#include "imgui.h"
#include "Engine/EntryPoint.h"
#include <Engine/Input/CameraController.h>

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