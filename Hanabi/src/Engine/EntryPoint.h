#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* CreateApplication();

int main(int argc, char** argv)
{
	Hanabi::Log::Init();
	auto app = CreateApplication();
	app->Run();
	delete app;
}

#endif 