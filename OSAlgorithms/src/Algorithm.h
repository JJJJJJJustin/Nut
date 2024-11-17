#pragma once

namespace Nut { namespace Tool {
	std::vector<std::pair<std::string, int>> ReadFile(const std::string& filename);
} }

namespace Nut
{

	// -------------------------------------------------------------------
	// -------------------- Experiment 1 ---------------------------------
	// -------------------------------------------------------------------
	struct Process {
	public:
		std::string name;      // ��������
		int priority;          // ���ȼ�
		int arrival_time;      // ����ʱ��
		int service_time;      // ����ʱ��
		int remaining_time;    // ʣ��ʱ��
		int turnaround_time;   // ��תʱ��
	public:
		Process(std::string n, int p, int at, int st)
			: name(n), priority(p), arrival_time(at), service_time(st), remaining_time(st), turnaround_time(0){}
	};

	class PriorityAlgorithm
	{
	public:
		// �ȽϺ������������ȼ�����
		bool compare(const Process& a, const Process& b);

		// �����������
		static void generate_processes(std::vector<Process>& processes, int num_processes);

		// ִ�����ȼ�����
		static std::vector<Process> priority_scheduling(std::vector<Process>& processes);
	};

	class RoundAlgorithm
	{
	public:
		// �����������
		static void generate_processes(std::vector<Process>& processes, int num_processes);

		// ʵ���㷨
		static std::vector<Process> round_robin(std::vector<Process>& processes, int time_quantum, const std::string& output_filename);
	};

	// -------------------------------------------------------------------
	// -------------------- Experiment 2 ---------------------------------
	// -------------------------------------------------------------------
	class Banker {
	public:
		Banker(int num, int type);
		void requestResources(int processId, std::vector<int> request);
		void releaseResources(int processId, std::vector<int> release);
		void printStatus();

	private:
		int m_Num; // ������
		int m_Type; // ��Դ����
		std::vector<std::vector<int>> m_Allocation; // �������
		std::vector<std::vector<int>> m_Max;        // ����������
		std::vector<std::vector<int>> m_Need;       // �������
		std::vector<int> m_Available;				// ������Դ
		bool isSafe();

		void calculateNeed();
	};


	// -------------------------------------------------------------------
	// -------------------- Experiment 3 ---------------------------------
	// -------------------------------------------------------------------

	class StorageManage 
	{
	public:
		StorageManage() = default;

		static void ParseInstructions(const std::string& filename, std::vector<std::string>& instructions);
		static int GetPage(int instructionIndex);

		static void GenerateInstructions(const std::string& filename, int totalInstructions = 320, int maxAddress = 1000);
		static std::vector<int> ExecuteInstructions(const std::string& filename);
	public:
		static double FIFO(const std::vector<int>& executedPages, int numFrames);
		static double LRU(const std::vector<int>& executedPages, int numFrames);
		static double OPT(const std::vector<int>& executedPages, int numFrames);
		static double Clock(const std::vector<int>& executedPages, int numFrames);

		static inline std::vector<int> GetPages() { return m_ExecutedPages; }
	private:
		static std::vector<int> m_ExecutedPages;

	};



	// ---------------------------------------------------------------------
	// ----------------------- Experiment 4 --------------------------------
	// ---------------------------------------------------------------------
	class DiskScheduler {
	private:
		std::vector<int> m_Requests;	// �ŵ������б�
		int m_Start;					// ��ͷ��ʼλ��
		int m_MaxTrack;					// ���ŵ�

	public:
		DiskScheduler(std::vector<int> reqs, int startPos, int maxTrack = 200);

		// SCAN �㷨ʵ��
		std::vector<int> Scan();

		// CSCAN �㷨ʵ��
		std::vector<int> Cscan();

		// �Ľ��� SCAN �㷨������ű�ճ��
		std::vector<int> ImprovedScan();

		// ��ӡ�ŵ�����˳��
		void Print(const std::vector<int>& sequence);
	};






}