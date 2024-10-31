#pragma once

namespace Nut { namespace Tool {
	std::vector<std::pair<std::string, int>> ReadFile(const std::string& filename);
} }

namespace Nut
{

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
}