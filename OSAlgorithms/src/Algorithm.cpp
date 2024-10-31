#include "nutpch.h"
#include "Algorithm.h"

#include "Nut/Scene/Scene.h"

#include <queue>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <limits>

#include <yaml-cpp/yaml.h>

// ------------------------------------------------------------------------------------------
// ------------------------------------ Tool ------------------------------------------------
// ------------------------------------------------------------------------------------------
std::vector<std::pair<std::string, int>> Nut::Tool::ReadFile(const std::string& filename)
{
	std::ifstream file(filename);
	std::string line;
	std::vector<std::pair<std::string, int>> results;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			// 查找特定的关键字
			int processPos = line.find("Running process: ");
			int timePos = line.find("executed for ");

			if (processPos != std::string::npos && timePos != std::string::npos) {
				// 提取进程名称
				int start = processPos + 17; // "Running process: " 的长度
				int end = line.find("(", start);
				std::string processName = line.substr(start, end - start - 1);

				// 提取执行时间
				int startTime = timePos + 13; // "executed for " 的长度
				int endTime = line.find("s", startTime);
				std::string executeTimeStr = line.substr(startTime, endTime - start);
				int executionTime = std::stoi(executeTimeStr);

				// 添加结果到 vector
				results.emplace_back(processName, executionTime);
			}
		}
	}
	else {
		NUT_CORE_ASSERT(file.is_open(), "Could not open file !");
	}

	return results; // 返回所有找到的结果
}

namespace Nut
{
	// -----------------------------------------------------------------------------------
	// ---------------------------- Priority Algorithm -----------------------------------
	// -----------------------------------------------------------------------------------
	bool PriorityAlgorithm::compare(const Process& a, const Process& b)
	{
		return a.priority > b.priority; // 优先级高的排在前面
	}

	void PriorityAlgorithm::generate_processes(std::vector<Process>& processes, int num_processes)
	{
		for (int i = 0; i < num_processes; ++i) {
			std::string name = "P" + std::to_string(i + 1);
			int priority = rand() % 10 + 1;       // 优先级范围 1-10
			int arrival_time = rand() % 10;       // 到达时间范围 0-9
			int service_time = rand() % 20 + 1;   // 服务时间范围 1-20
			processes.emplace_back(name, priority, arrival_time, service_time);
		}
	}

	std::vector<Process> PriorityAlgorithm::priority_scheduling(std::vector<Process>& processes)
	{
		std::vector<Process> completed_processes;	// 完成的进程
		int time = 0;								// 当前时间

		while (!processes.empty()) {
			// 将已到达的进程添加到就绪队列
			std::vector<Process*> ready_queue;
			for (auto& process : processes) {
				if (process.arrival_time <= time) {
					ready_queue.push_back(&process);
				}
			}

			// 如果就绪队列不为空
			if (!ready_queue.empty()) {
				// 根据优先级排序
				std::sort(ready_queue.begin(), ready_queue.end(),
					[](Process* a, Process* b) { return a->priority > b->priority; });

				// 选择优先级最高的进程
				Process* current_process = ready_queue.front();
				current_process->remaining_time--;

				std::cout << "Time: " << time << " - Running process: " << current_process->name << std::endl;

				// 如果当前进程完成
				if (current_process->remaining_time == 0) {
					// 计算周转时间
					current_process->turnaround_time = time + 1 - current_process->arrival_time; // +1 是因为时间从0开始

					completed_processes.push_back(*current_process);
					processes.erase(std::remove_if(processes.begin(), processes.end(),
						[current_process](Process& p) { return p.name == current_process->name; }), processes.end());
				}
			}
			else {
				std::cout << "Time: " << time << " - No process is running." << std::endl;
			}

			time++; // 时间推进
		}

		// 输出完成的进程顺序及服务时间和周转时间
		std::cout << "\nCompleted Processes in Order:\n";
		for (const auto& process : completed_processes) {
			std::cout << process.name << " - Service Time: " << process.service_time
				<< " - Turnaround Time: " << process.turnaround_time << std::endl;
		}

		return completed_processes;
	}


	// -----------------------------------------------------------------------------------
	// --------------------------- Round-robin -------------------------------------------
	// -----------------------------------------------------------------------------------
	//随机生成进程
	void RoundAlgorithm::generate_processes(std::vector<Process>& processes, int num_processes)
	{
		for (int i = 0; i < num_processes; ++i) {
			std::string name = "P" + std::to_string(i + 1);
			int priority = rand() % 10 + 1;       // 优先级范围 1-10
			int arrival_time = rand() % 10;       // 到达时间范围 0-9
			int service_time = rand() % 20 + 1;   // 服务时间范围 1-20
			processes.emplace_back(name, priority, arrival_time, service_time);
		}

	}

	// 轮转调度算法
	std::vector<Process> RoundAlgorithm::round_robin(std::vector<Process>& processes, int time_segment, const std::string& output_filename) {
		std::queue<int> ready_queue; // 就绪队列
		int current_time = 0;
		int n = processes.size();
		int processes_completed = 0;

		// 排序进程根据到达时间
		std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });

		#// 打开文件输出流
		std::ofstream output_file(output_filename);
		NUT_CORE_ASSERT(output_file.is_open(), "Failed to open file ! ")

		while (processes_completed < n) {
			// 将到达的进程加入就绪队列
			for (int i = 0; i < n; ++i) {
				if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
					ready_queue.push(i);
				}
			}

			if (!ready_queue.empty()) {
				int current_process_index = ready_queue.front();
				ready_queue.pop();

				// 检查当前进程是否仍然需要执行
				if (processes[current_process_index].remaining_time > 0) {
					// 输出当前时间和正在运行的进程
					output_file << "Time: " << current_time << " - Running process: " << processes[current_process_index].name;

					// 处理当前进程
					int execution_time = std::min(processes[current_process_index].remaining_time, time_segment);
					processes[current_process_index].remaining_time -= execution_time;
					current_time += execution_time;

					// 输出当前进程执行的时间
					output_file << " ( executed for " << execution_time << "s";

					if (processes[current_process_index].remaining_time == 0) {
						processes[current_process_index].turnaround_time = current_time - processes[current_process_index].arrival_time;
						output_file << " and finished.\n"; // 输出完成信息
						processes_completed++;
					}
					else {
						output_file << " and not finished.\n"; // 输出未完成信息
						ready_queue.push(current_process_index);
					}
				}

				// 如果进程未完成，重新加入就绪队列
				if (processes[current_process_index].remaining_time > 0) {
					ready_queue.push(current_process_index);
				}
			}
			else {
				// 如果没有进程可执行，则跳到下一个可到达进程
				current_time++;
			}
		}

		// 关闭文件输出流
		output_file.close();
		return processes;
	}


	// -----------------------------------------------------------------------------------
	// --------------------------- Banker ------------------------------------------------
	// -----------------------------------------------------------------------------------

	class Banker {
	public:
		Banker(int n, int m);
		void requestResources(int processId, std::vector<int> request);
		void releaseResources(int processId, std::vector<int> release);
		void printStatus();

	private:
		int n; // 进程数
		int m; // 资源种类
		std::vector<std::vector<int>> allocation; // 分配矩阵
		std::vector<std::vector<int>> max;        // 最大需求矩阵
		std::vector<std::vector<int>> need;       // 需求矩阵
		std::vector<int> available;          // 可用资源
		bool isSafe();

		void calculateNeed();
	};

	Banker::Banker(int numProcesses, int numResources) : n(numProcesses), m(numResources) {
		allocation.resize(n, std::vector<int>(m));
		max.resize(n, std::vector<int>(m));
		need.resize(n, std::vector<int>(m));
		available.resize(m);

		srand(time(0));

		// 随机生成可用资源
		for (int j = 0; j < m; j++) {
			available[j] = rand() % 20 + 1;     // 可用资源范围 1~20
		}

		// 随机生成分配矩阵和最大需求矩阵
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				allocation[i][j] = rand() % 20; // 资源分配范围 0~19
				max[i][j] = allocation[i][j] + (rand() % (20 - allocation[i][j])); // 最大需求保证合理性
			}
		}

		calculateNeed();
	}

	void Banker::calculateNeed() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				need[i][j] = max[i][j] - allocation[i][j];
			}
		}
	}

	bool Banker::isSafe() {
		std::vector<int> work = available;
		std::vector<bool> finish(n, false);
		std::vector<int> safeSequence;

		while (safeSequence.size() < n) {
			bool found = false;
			for (int i = 0; i < n; i++) {
				if (!finish[i]) {
					bool canAllocate = true;
					for (int j = 0; j < m; j++) {
						if (need[i][j] > work[j]) {
							canAllocate = false;
							break;
						}
					}
					if (canAllocate) {
						for (int j = 0; j < m; j++) {
							work[j] += allocation[i][j];
						}
						finish[i] = true;
						safeSequence.push_back(i);
						found = true;
					}
				}
			}
			if (!found) {
				return false;       // 找不到安全序列
			}
		}

		// 输出安全序列
		std::cout << "安全序列: ";
		for (int i : safeSequence) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		return true;
	}

	void Banker::requestResources(int processId, std::vector<int> request) {
		// 检查请求是否合法
		for (int j = 0; j < m; j++) {
			if (request[j] > need[processId][j] || request[j] > available[j]) {
				std::cout << "进程 " << processId << " 的请求无效！" << std::endl;
				return;
			}
		}

		// 临时分配资源
		for (int j = 0; j < m; j++) {
			available[j] -= request[j];
			allocation[processId][j] += request[j];
			need[processId][j] -= request[j];
		}

		// 检查系统是否安全
		if (!isSafe()) {
			// 如果不安全，回滚
			for (int j = 0; j < m; j++) {
				available[j] += request[j];
				allocation[processId][j] -= request[j];
				need[processId][j] += request[j];
			}
			std::cout << "进程 " << processId << " 的请求导致不安全，回滚！" << std::endl;
		}
	}

	void Banker::releaseResources(int processId, std::vector<int> release) {
		for (int j = 0; j < m; j++) {
			allocation[processId][j] -= release[j];
			available[j] += release[j];
		}
		calculateNeed();
		std::cout << "进程 " << processId << " 释放资源成功！" << std::endl;
	}

	void Banker::printStatus() {
		std::cout << "资源分配状况:" << std::endl;
		std::cout << "进程\t分配\t\t最大需求\t需求" << std::endl;
		for (int i = 0; i < n; i++) {
			std::cout << i << "\t";
			for (int j = 0; j < m; j++) {
				std::cout << allocation[i][j] << " ";
			}
			std::cout << "\t";
			for (int j = 0; j < m; j++) {
				std::cout << max[i][j] << " ";
			}
			std::cout << "\t";
			for (int j = 0; j < m; j++) {
				std::cout << need[i][j] << " ";
			}
			std::cout << std::endl;
		}

		std::cout << "可用资源: ";
		for (int j = 0; j < m; j++) {
			std::cout << available[j] << " ";
		}
		std::cout << std::endl;
	}



}