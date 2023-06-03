#pragma once
#include "Engine/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Hanabi
{
	enum class LogLevel : int32_t
	{
		Trace = BIT(0),
		Info = BIT(1),
		Warn = BIT(2),
		Error = BIT(3),
		Critical = BIT(4)
	};

	class Log
	{
	public:
		static void Init();
		static void Showdown();	
		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		static Ref<spdlog::logger>& GetScriptLogger() { return s_ScriptLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
		static Ref<spdlog::logger> s_ScriptLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
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

//Script log macros
#define HNB_SCRIPT_TRACE(...) ::Hanabi::Log::GetScriptLogger()->trace(__VA_ARGS__)
#define HNB_SCRIPT_INFO(...) ::Hanabi::Log::GetScriptLogger()->info(__VA_ARGS__)
#define HNB_SCRIPT_WARN(...) ::Hanabi::Log::GetScriptLogger()->warn(__VA_ARGS__)
#define HNB_SCRIPT_ERROR(...) ::Hanabi::Log::GetScriptLogger()->error(__VA_ARGS__)
#define HNB_SCRIPT_CRITICAL(...) ::Hanabi::Log::GetScriptLogger()->critical(__VA_ARGS__)