#include <Hanabi.h>

class Sandbox :public Hanabi::Application
{
public :
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

Hanabi::Application* CreateApplication()
{
	return new Sandbox();
}