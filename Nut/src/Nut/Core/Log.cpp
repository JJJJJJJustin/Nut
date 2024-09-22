#include "nutpch.h"
#include "Nut/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Nut {

	Ref<spdlog::logger> Log::s_CoreLogger;					//��̬��Ա��������Ҫ���ж��壨���崦����ѡ���Ƿ���г�ʼ����
	Ref<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		//LogSink����־Ŀ�꣩: ��ʾ��־��Ϣ�������ʽ��洢λ�ã�������洢������ָ�� shared_ptr ��
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());						// �洢һ������־��Ϣ���������̨,��������ɫ����־Ŀ�ꡣ
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Nut.log", true));		// ָ��һ����־Ŀ�꣬����� "Nut.log" ������ÿ������ʱ����ļ� (true ��ʾ��գ�false ��ʾ׷��д��)��

		//��ʽ���
		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		//������־��¼��
		s_CoreLogger = std::make_shared<spdlog::logger>("NUT", begin(logSinks), end(logSinks));			// Logger with range on sinks: logger(std::string name, It begin, It end) ����������Ҫʹ�õ���־Ŀ��
		spdlog::register_logger(s_CoreLogger);																// Register the given logger with the given name(ʹ�ø���������ע������ļ�¼��)
		s_CoreLogger->set_level(spdlog::level::trace);														// Core_Logger ��¼ Trace �����ϼ������Ϣ
		s_CoreLogger->flush_on(spdlog::level::trace);														// ��¼��־��Ϣʱ����������������־ˢ�µ����Ŀ��,��ֹ�������������Ϣ��ʧ

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
	
}