#include <Hanabi.h>
#include "Engine/EntryPoint.h"
#include "ExampleLayer2D.h"
#include "ExampleLayer3D.h"

class Sandbox :public Hanabi::Application
{
public:
	Sandbox() :Hanabi::Application("SandBox")
	{
		PushLayer(new ExampleLayer3D());
		//PushLayer(new ExampleLayer2D());
	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}