#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* CreateApplication();

int main(int argc, char** argv)
{
	Hanabi::Log::Init();
	HNB_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = CreateApplication();
	HNB_PROFILE_END_SESSION();

	HNB_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HNB_PROFILE_END_SESSION();

	HNB_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
	delete app;
	HNB_PROFILE_END_SESSION();
}

#endif 