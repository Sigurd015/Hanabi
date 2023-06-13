#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* Hanabi::CreateApplication(Hanabi::ApplicationCommandLineArgs args);

namespace Hanabi
{
	int Main(int argc, char** argv)
	{
		InitializeCore();
		auto app = CreateApplication({ argc, argv });
		app->Run();
		delete app;
		ShutdownCore();
		return 0;
	}
}

#ifdef HNB_DIST

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Hanabi::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Hanabi::Main(argc, argv);
}

#endif

#endif 