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
		std::string name;      // 进程名称
		int priority;          // 优先级
		int arrival_time;      // 到达时间
		int service_time;      // 服务时间
		int remaining_time;    // 剩余时间
		int turnaround_time;   // 周转时间
	public:
		Process(std::string n, int p, int at, int st)
			: name(n), priority(p), arrival_time(at), service_time(st), remaining_time(st), turnaround_time(0){}
	};

	class PriorityAlgorithm
	{
	public:
		// 比较函数，用于优先级排序
		bool compare(const Process& a, const Process& b);

		// 生成随机进程
		static void generate_processes(std::vector<Process>& processes, int num_processes);

		// 执行优先级调度
		static std::vector<Process> priority_scheduling(std::vector<Process>& processes);
	};

	class RoundAlgorithm
	{
	public:
		// 生成随机进程
		static void generate_processes(std::vector<Process>& processes, int num_processes);

		// 实现算法
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
		int m_Num; // 进程数
		int m_Type; // 资源种类
		std::vector<std::vector<int>> m_Allocation; // 分配矩阵
		std::vector<std::vector<int>> m_Max;        // 最大需求矩阵
		std::vector<std::vector<int>> m_Need;       // 需求矩阵
		std::vector<int> m_Available;				// 可用资源
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
		std::vector<int> m_Requests;	// 磁道请求列表
		int m_Start;					// 磁头初始位置
		int m_MaxTrack;					// 最大磁道

	public:
		DiskScheduler(std::vector<int> reqs, int startPos, int maxTrack = 200);

		// SCAN 算法实现
		std::vector<int> Scan();

		// CSCAN 算法实现
		std::vector<int> Cscan();

		// 改进的 SCAN 算法，避免磁臂粘着
		std::vector<int> ImprovedScan();

		// 打印磁道访问顺序
		void Print(const std::vector<int>& sequence);
	};






}