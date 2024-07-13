#include "nutpch.h"
#include "Nut/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Nut {

	Ref<spdlog::logger> Log::s_CoreLogger;					//静态成员变量必须要进行定义（定义处可以选择是否进行初始化）
	Ref<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Nut");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}