#include "hnbpch.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Hanabi
{
	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_ClientLogger;
	Ref<spdlog::logger> Log::s_ScriptLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Hanabi.log", true));

		logSinks[0]->set_pattern("%^>[%T]-[%n]: %v%$");
		logSinks[1]->set_pattern("%^>[%T]-[%n]: %v%$");

		s_CoreLogger = std::make_shared<spdlog::logger>("ENGINE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
		s_ScriptLogger = std::make_shared<spdlog::logger>("SCRIPT", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ScriptLogger);
		s_ScriptLogger->set_level(spdlog::level::trace);
		s_ScriptLogger->flush_on(spdlog::level::trace);
	}

	void Log::Showdown()
	{
		s_CoreLogger.reset();
		s_ClientLogger.reset();
		s_ScriptLogger.reset();
	}
}