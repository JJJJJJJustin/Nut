#include "nutpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Nut {

	std::shared_ptr <spdlog::logger> Log::s_CoreLogger;					//��̬��Ա��������Ҫ���ж��壨���崦����ѡ���Ƿ���г�ʼ����
	std::shared_ptr <spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Nut");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}