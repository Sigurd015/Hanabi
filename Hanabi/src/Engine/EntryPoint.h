#pragma once

#ifdef HNB_PLATFORM_WINDOWS

extern Hanabi::Application* CreateApplication();

int main(int argc, char** argv)
{
	Hanabi::Log::Init();
	HNB_CORE_WARN("initialized spdlog");
	HNB_CORE_INFO("initialized spdlog");
	HNB_CORE_ERROR("initialized spdlog");
	HNB_CORE_TRACE("initialized spdlog");
	HNB_CORE_FATAL("initialized spdlog");
	
	int a = 5;
	HNB_WARN("h!var={0}", a);
	HNB_INFO("h!var={0}", a);
	HNB_ERROR("h!var={0}", a);
	HNB_TRACE("h!var={0}", a);
	HNB_FATAL("h!var={0}", a);
	auto app = CreateApplication();
	app->Run();
	delete app;
}

#endif 


