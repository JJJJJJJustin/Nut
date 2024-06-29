#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Nut {
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    public:
        Instrumentor()                                                                  // Why not Private ? 
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")    // �Ự���ڽϸ߲�εĺ���(In Entry Point),�����ڽϵͲ�εĺ���������Sandbox �еĻ��Ʋ�����
        {                                                                                           // һ���Ự(Session),�������(InstrumentorTimer)
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }

        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)                                              // JSON �ļ�����
        {
            if (m_ProfileCount++ > 0)                                                               // �ȱȽϣ�������
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();                                                                     //����ˢ�½��ļ����������ӳٴ�����Ӱ�죨ʱ�����ƫ�
        }

        void WriteHeader()                                                                              // JSON �ļ�ͷ��
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()                                                                              // JSON �ļ�β��
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()                                                                      // Only One instance so use static(Singleton ����ģʽ)
        {
            static Instrumentor instance;
            return instance;
        }
    };

    // ------------------------- Timer ---------------------------------------
    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());                   //�� Thread ID ӳ��Ϊ��ϣֵ��Ȼ�󴫵ݸ� threadID
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });                             // In "Instrumentor"

            m_Stopped = true;
        }
    private:
        bool m_Stopped;
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    };
}

#define NUT_PROFILE 1
#if NUT_PROFILE
    #define NUT_PROFILE_BEGIN_SESSION(name, filepath) ::Nut::Instrumentor::Get().BeginSession(name, filepath)
    #define NUT_PROFILE_END_SESSION() ::Nut::Instrumentor::Get().EndSession()
    #define NUT_PROFILE_SCOPE(name) ::Nut::InstrumentationTimer timer##__LINE__(name)
    #define NUT_PROFILE_FUNCTION() NUT_PROFILE_SCOPE(__FUNCSIG__)
#else
    #define NUT_PROFILE_BEGIN_SESSION(name, filepath) 
    #define NUT_PROFILE_END_SESSION() 
    #define NUT_PROFILE_SCOPE(name) 
    #define NUT_PROFILE_FUNCTION() 
#endif