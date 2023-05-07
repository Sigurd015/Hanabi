#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* Hanabi::CreateApplication(Hanabi::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Hanabi::Log::Init();

	HNB_PROFILE_BEGIN_SESSION("Startup", "HanabiProfile-Startup.json");
	auto app = Hanabi::CreateApplication({ argc, argv });
	HNB_PROFILE_END_SESSION();

	HNB_PROFILE_BEGIN_SESSION("Runtime", "HanabiProfile-Runtime.json");
	app->Run();
	HNB_PROFILE_END_SESSION();

	HNB_PROFILE_BEGIN_SESSION("Shutdown", "HanabiProfile-Shutdown.json");
	delete app;
	HNB_PROFILE_END_SESSION();
}

#endif 