#pragma once
#include "Engine/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Hanabi
{
	class Log
	{
	public:
		static void Init();
		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define HNB_CORE_TRACE(...) ::Hanabi::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HNB_CORE_INFO(...) ::Hanabi::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HNB_CORE_WARN(...) ::Hanabi::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HNB_CORE_ERROR(...) ::Hanabi::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HNB_CORE_CRITICAL(...) ::Hanabi::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client log macros
#define HNB_TRACE(...) ::Hanabi::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HNB_INFO(...) ::Hanabi::Log::GetClientLogger()->info(__VA_ARGS__)
#define HNB_WARN(...) ::Hanabi::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HNB_ERROR(...) ::Hanabi::Log::GetClientLogger()->error(__VA_ARGS__)
#define HNB_CRITICAL(...) ::Hanabi::Log::GetClientLogger()->critical(__VA_ARGS__)