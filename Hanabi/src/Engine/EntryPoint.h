#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* Hanabi::CreateApplication(Hanabi::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Hanabi::InitializeCore();
	auto app = Hanabi::CreateApplication({ argc, argv });
	app->Run();
	delete app;
	Hanabi::ShutdownCore();
}

#endif 