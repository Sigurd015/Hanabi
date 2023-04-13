#include <Hanabi.h>

class Sandbox :public Hanabi::Application
{
public:
	Sandbox()
	{
		PushOverlay(new Hanabi::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}