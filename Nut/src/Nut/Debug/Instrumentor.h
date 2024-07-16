#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>

namespace Nut {
    struct ProfileResult
    {
        std::string Name;
        std::chrono::duration<double, std::micro> Start;
        std::chrono::microseconds ElapsedTime;
        std::thread::id ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        std::mutex m_Mutex;                                                             // ���뻥����

        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
    public:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")    // �Ự���ڽϸ߲�εĺ���(In Entry Point),�����ڽϵͲ�εĺ���������Sandbox �еĻ��Ʋ�����
        {                                                                                           // һ���Ự(Session),�������(InstrumentorTimer)
            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession) {
                // �����ǰ�Ự�Ѿ����ڣ����ڿ�ʼ�»Ự֮ǰ����رա�
                // ���ԭʼ�Ự�ĺ���������������ճ������´򿪵ĻỰ�С�
                // ��ȸ�ʽ����ķ������Ҫ�á�
                // Edge case: BeginSession() might be before Log::Init()
                if (Log::GetCoreLogger()) { NUT_CORE_ERROR("Instrumentor::BeginSession('{0}') is creating a new session when session '{1}' already open.", name, m_CurrentSession->Name); }
                InternalEndSession();
            }
            m_OutputStream.open(filepath);
            if (m_OutputStream.is_open()) {
                m_CurrentSession = new InstrumentationSession({ name });                              // InstrumentationSession create mutex first and then WriteHeader manage the lock by std::lock_guard
                WriteHeader();
            } else {
                // Edge case : BeginSession() might be before Log::Init() !!!
                if (Log::GetCoreLogger()) { NUT_CORE_ERROR("Instrumentor can't open result file '{0}'.", filepath); }
            }
        }

        void EndSession()                                                                           // Use RAII so manage thread in Session
        {
            // Must ensure that the mutex has been created when you call InternalEndSession
            std::lock_guard lock(m_Mutex);
            InternalEndSession();
        }
    private:
        void InternalEndSession() {                                                                 // End Session logicly
            if (m_CurrentSession) {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }

        void WriteHeader()                                                                              // JSON �ļ�ͷ��
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        void WriteFooter()                                                                              // JSON �ļ�β��
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }
    public:
        void WriteProfile(const ProfileResult& result)                                              // JSON �ļ�����
        {
            std::stringstream json;

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            json << std::setprecision(3) << std::fixed;                                             // ����json �ַ������и��������������С���������λ������С����̶��ĸ�ʽ
            json << ",{";                                                                           // Ĭ�����һ��','���ʲ����ж��Ƿ���Ҫ','������Header�ж�д��һ��{}���ʳ�ʼ��ʱ��һ��','�������ࡣ
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << result.ElapsedTime.count() << ',';
            json << "\"name\":\"" << name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession) {
                m_OutputStream << json.str();
                m_OutputStream.flush();                                                                 //����ˢ�½��ļ����������ӳٴ�����Ӱ�죨ʱ�����ƫ�
            }

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
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();

            //long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            //long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            auto highResStart = std::chrono::duration<double, std::micro>{ m_StartTimepoint.time_since_epoch() };
            auto elapsedTime  = 
              std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() 
                - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            Instrumentor::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id()});

            m_Stopped = true;
        }
    private:
        bool m_Stopped;
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    };
}

// ------------------------------- Macro --------------------------------------------------------
#define NUT_PROFILE 1

#if NUT_PROFILE
    // Resolve which function signature macro will be used. Note that this only is resolved when the (pre)compiler starts, 
    // so the syntax highlighting could mark the wrong one in your editor!
    // ���ݲ�ͬ�����ϵı༭������汾ȷ����Ӧ�ĺ��ʵĻ�ȡ��ʽ�����䶨��Ϊ NUT_FUNC_SIG���Զ���ȡ����ǩ��
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define NUT_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define NUT_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__FUNCSIG__)
        #define NUT_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define NUT_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define NUT_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define NUT_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define NUT_FUNC_SIG __func__
    #else
        #define NUT_FUNC_SIG "NUT_FUNC_SIG unknown!"
    #endif


    #define NUT_PROFILE_BEGIN_SESSION(name, filepath) ::Nut::Instrumentor::Get().BeginSession(name, filepath)
    #define NUT_PROFILE_END_SESSION() ::Nut::Instrumentor::Get().EndSession()
    #define NUT_PROFILE_SCOPE(name) ::Nut::InstrumentationTimer timer##__LINE__(name)
    #define NUT_PROFILE_FUNCTION() NUT_PROFILE_SCOPE(NUT_FUNC_SIG)
#else
    #define NUT_PROFILE_BEGIN_SESSION(name, filepath) 
    #define NUT_PROFILE_END_SESSION() 
    #define NUT_PROFILE_SCOPE(name) 
    #define NUT_PROFILE_FUNCTION() 
#endif