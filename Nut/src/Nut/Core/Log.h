#pragma once

#include "Nut/Core/Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Nut {

	class Log
	{
	private:
		static Ref<spdlog::logger> s_CoreLogger;			//��̬��Ա����ֻ�ܷ��ʾ�̬��Ա����
		static Ref<spdlog::logger> s_ClientLogger;
	public:
		static void Init();
		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	};

}

//core log macros
#define NUT_CORE_TRACE(...)\
	::Nut::Log::GetCoreLogger()->trace(__VA_ARGS__);	//????why before the Nut needs "::",it seems doesn't matter just now( at Episode 6 )����ʾ��ȫ���е������GetCoreLogger
#define NUT_CORE_INFO(...)\
	::Nut::Log::GetCoreLogger()->info(__VA_ARGS__);		//(...)��ʾ�꺯�����Խ������������
#define NUT_CORE_WARN(...)\
	::Nut::Log::GetCoreLogger()->warn(__VA_ARGS__);		//__VA_ARGS__��һ��Ԥ����ĺ꣨ǰ���˫�»��߱�ʾ����һ��Ԥ����ģ�������������̬�Ľ��յ�δ֪��������
#define NUT_CORE_ERROR(...)\
	::Nut::Log::GetCoreLogger()->error(__VA_ARGS__);
#define NUT_CORE_CRITICAL(...)\
	::Nut::Log::GetCoreLogger()->critical(__VA_ARGS__);

//client log macros
#define NUT_TRACE(...)\
	::Nut::Log::GetClientLogger()->trace(__VA_ARGS__);
#define NUT_INFO(...)\
	::Nut::Log::GetClientLogger()->info(__VA_ARGS__);
#define NUT_WARN(...)\
	::Nut::Log::GetClientLogger()->warn(__VA_ARGS__);
#define NUT_ERROR(...)\
	::Nut::Log::GetClientLogger()->error(__VA_ARGS__);
#define NUT_CRITICAL(...)\
	::Nut::Log::GetClientLogger()->critical(__VA_ARGS__);