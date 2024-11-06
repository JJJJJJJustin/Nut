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
	Banker::Banker(int num, int type) : m_Num(num), m_Type(type) 
	{
		m_Allocation.resize(m_Num, std::vector<int>(m_Type));
		m_Max.resize(m_Num, std::vector<int>(m_Type));
		m_Need.resize(m_Num, std::vector<int>(m_Type));
		m_Available.resize(m_Type);

		srand(time(0));

		// 随机生成可用资源
		for (int j = 0; j < m_Type; j++) {
			m_Available[j] = rand() % 20 + 1;     // 可用资源范围 1~20
		}

		// 随机生成分配矩阵和最大需求矩阵
		for (int i = 0; i < m_Num; i++) {
			for (int j = 0; j < m_Type; j++) {
				m_Allocation[i][j] = rand() % 20; // 资源分配范围 0~19
				m_Max[i][j] = m_Allocation[i][j] + (rand() % (20 - m_Allocation[i][j])); // 最大需求保证合理性
			}
		}

		calculateNeed();
	}

	void Banker::calculateNeed() {
		for (int i = 0; i < m_Num; i++) {
			for (int j = 0; j < m_Type; j++) {
				m_Need[i][j] = m_Max[i][j] - m_Allocation[i][j];
			}
		}
	}

	bool Banker::isSafe() {
		std::vector<int> work = m_Available;
		std::vector<bool> finish(m_Num, false);
		std::vector<int> safeSequence;

		while (safeSequence.size() < m_Num) {
			bool found = false;
			for (int i = 0; i < m_Num; i++) {
				if (!finish[i]) {
					bool canAllocate = true;
					for (int j = 0; j < m_Type; j++) {
						if (m_Need[i][j] > work[j]) {
							canAllocate = false;
							break;
						}
					}
					if (canAllocate) {
						for (int j = 0; j < m_Type; j++) {
							work[j] += m_Allocation[i][j];
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
		for (int j = 0; j < m_Type; j++) {
			if (request[j] > m_Need[processId][j] || request[j] > m_Available[j]) {
				std::cout << "进程 " << processId << " 的请求无效！" << std::endl;
				return;
			}
		}

		// 临时分配资源
		for (int j = 0; j < m_Type; j++) {
			m_Available[j] -= request[j];
			m_Allocation[processId][j] += request[j];
			m_Need[processId][j] -= request[j];
		}

		// 检查系统是否安全
		if (!isSafe()) {
			// 如果不安全，回滚
			for (int j = 0; j < m_Type; j++) {
				m_Available[j] += request[j];
				m_Allocation[processId][j] -= request[j];
				m_Need[processId][j] += request[j];
			}
			std::cout << "进程 " << processId << " 的请求导致不安全，回滚！" << std::endl;
		}
	}

	void Banker::releaseResources(int processId, std::vector<int> release) {
		for (int j = 0; j < m_Type; j++) {
			m_Allocation[processId][j] -= release[j];
			m_Available[j] += release[j];
		}
		calculateNeed();
		std::cout << "进程 " << processId << " 释放资源成功！" << std::endl;
	}

	void Banker::printStatus() {
		std::cout << "资源分配状况:" << std::endl;
		std::cout << "进程\t分配\t\t最大需求\t需求" << std::endl;
		for (int i = 0; i < m_Num; i++) {
			std::cout << i << "\t";
			for (int j = 0; j < m_Type; j++) {
				std::cout << m_Allocation[i][j] << " ";
			}
			std::cout << "\t";
			for (int j = 0; j < m_Type; j++) {
				std::cout << m_Max[i][j] << " ";
			}
			std::cout << "\t";
			for (int j = 0; j < m_Type; j++) {
				std::cout << m_Need[i][j] << " ";
			}
			std::cout << std::endl;
		}

		std::cout << "可用资源: ";
		for (int j = 0; j < m_Type; j++) {
			std::cout << m_Available[j] << " ";
		}
		std::cout << std::endl;
	}


	// ---------------------------------------------------------------
	// ---------------------- Storage Management ---------------------
	// ---------------------------------------------------------------
	std::vector<int> StorageManage::m_ExecutedPages;					// 定义静态成员变量

	void StorageManage::GenerateInstructions(const std::string& filename, int totalInstructions, int maxAddress) {
		// 打开文件
		std::ofstream file(filename);
		NUT_CORE_ASSERT(file, "无法打开文件");

		// 设置随机数种子
		std::srand(std::time(nullptr));

		// 计算每类指令的数量
		int numSequential = totalInstructions / 2;
		int numFront = totalInstructions / 4;
		int numBack = totalInstructions / 4;

		// 生成顺序执行的指令
		for (int i = 0; i < numSequential; ++i) {
			int address = i;  // 顺序地址
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// 生成均匀分布在前地址部分的指令
		for (int i = 0; i < numFront; ++i) {
			int address = std::rand() % (maxAddress / 2);  // 前半部分的地址
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// 生成均匀分布在后地址部分的指令
		for (int i = 0; i < numBack; ++i) {
			int address = std::rand() % (maxAddress / 2) + (maxAddress / 2);  // 后半部分的地址
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// 关闭文件
		file.close();
		NUT_CORE_INFO("文件生成完成，包含{0}条指令。", totalInstructions)
	}

	std::vector<int> StorageManage::ExecuteInstructions(const std::string& filename) {
		// 读取指令
		std::vector<std::string> instructions;
		ParseInstructions(filename, instructions);

		// 结果保存
		m_ExecutedPages.reserve(320); // 执行320条指令

		// 随机数种子
		std::srand(std::time(nullptr));

		// 随机选取一个起始点m（指令地址）
		int m = std::rand() % 320;  // 随机选取起始指令地址 m (范围 0 到 319)

		// 执行320条指令
		for (int i = 0; i < 320; ++i) {
			// 顺序执行地址为 m + 1 的指令
			m = (m + 1) % 320;  // 顺序执行一条指令，即 m+1 地址
			m_ExecutedPages.push_back(GetPage(m));

			// 在前地址 [0, m+1] 中随机选取一条指令
			int m_prime = std::rand() % (m + 1);  // 随机选取一个地址
			m_ExecutedPages.push_back(GetPage(m_prime));

			// 顺序执行地址为 m' + 1 的指令
			m_prime = (m_prime + 1) % 320;  // 顺序执行一条指令
			m_ExecutedPages.push_back(GetPage(m_prime));

			// 在后地址 [m'+2, 319] 中随机选取一条指令
			int m_double_prime = std::rand() % (320 - (m_prime + 2)) + (m_prime + 2);
			m_ExecutedPages.push_back(GetPage(m_double_prime));

			// 更新m
			m = m_double_prime;
		}

		// 打印页面流
		std::cout << "Executed Pages: ";
		for (int page : m_ExecutedPages) {
			std::cout << page << " ";
		}
		std::cout << std::endl;

		return m_ExecutedPages;
	}

	void StorageManage::ParseInstructions(const std::string& filename, std::vector<std::string>& instructions) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Unable to open file: " << filename << std::endl;
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			instructions.push_back(line);
		}
		file.close();
	}

	int StorageManage::GetPage(int instructionIndex) {
		// 每页10条指令
		return instructionIndex / 10;
	}


	// --------------- 4 Algorithm ------------------------
	 // FIFO算法实现
	double StorageManage::FIFO(const std::vector<int>& executedPages, int numFrames) {
		std::deque<int> pageFrames;								// 双端队列：内存中存储的页面，队首表示最早被访问的页面，队尾表示最近访问的页面
		std::unordered_map<int, bool> pageSet;
		int pageFaults = 0;

		for (int page : executedPages) {
			if (pageSet.find(page) == pageSet.end()) {
				pageFaults++;
				if (pageFrames.size() == numFrames) {			//	若内存已满，则替换最早被访问的页面
					int oldest = pageFrames.front();
					pageFrames.pop_front();
					pageSet.erase(oldest);
				}
				pageFrames.push_back(page);						// 将当前页面添加到 pageFrames 的队尾，表示该页面是最近访问的页面
				pageSet[page] = true;							// 标记该页面已被加载
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}

	// LRU算法实现
	double StorageManage::LRU(const std::vector<int>& executedPages, int numFrames) {
		std::unordered_map<int, std::list<int>::iterator> pageMap;		// 物理块列表中的页面，存储页面与其在双向链表中的位置映射
		std::list<int> pageFrames;										// 双向链表，用来维护页面的访问顺序（链表头为最近访问的页面）
		int pageFaults = 0;												// 发生缺页的页面数

		for (int page : executedPages) {
			if (pageMap.find(page) == pageMap.end()) {
				pageFaults++;
				if (pageFrames.size() == numFrames) {					// 如果内存已满，需要替换页面
					int oldest = pageFrames.back();
					pageFrames.pop_back();								// 从链表中移除最久未访问的页面
					pageMap.erase(oldest);								// 从无序哈希表中删除该页面
				}
			}
			else {
				pageFrames.erase(pageMap[page]);						// 页面已经在内存中，先从链表中移除该页面
			}

			pageFrames.push_front(page);								// 将当前页面移到链表的头部，表示它是最新被访问的
			pageMap[page] = pageFrames.begin();							// 更新页面映射，指向链表头部
		}

		return 1.0 - (double)pageFaults / executedPages.size();			// 1 - （缺页数/页面流总数）
	}

	// OPT算法实现
	double StorageManage::OPT(const std::vector<int>& executedPages, int numFrames) {
		std::vector<int> pageFrames;									// pageFrames 用来存储当前在内存中的页面（物理块中）

		// pageFaults 记录页面缺失次数
		int pageFaults = 0;

		// 遍历所有访问的页面
		for (int i = 0; i < executedPages.size(); ++i) {
			int page = executedPages[i];

			// 如果当前页面不在内存中，发生页面缺失
			if (std::find(pageFrames.begin(), pageFrames.end(), page) == pageFrames.end()) {
				pageFaults++;

				// 如果内存已满，选择一个页面进行替换
				if (pageFrames.size() == numFrames) {
					int farthest = -1;					// 记录当前页面在未来访问序列中最远的访问位置
					int pageToReplace = -1;				// 记录需要替换的页面

					// 遍历所有已加载的页面，找出最久未使用的页面
					for (int j = 0; j < pageFrames.size(); ++j) {
						int k;
						// 寻找该页面在之后的访问序列中出现的下一个位置
						for (k = i + 1; k < executedPages.size(); ++k) {
							if (executedPages[k] == pageFrames[j]) {
								// 找到了该页面，跳出内层循环
								break;
							}
						}

						// 如果该页面之后再也不会被访问，则优先替换该页面
						if (k == executedPages.size()) {
							pageToReplace = pageFrames[j];
							break;
						}

						// 如果该页面之后还会被访问，记录它的下次访问位置
						if (k > farthest) {
							farthest = k;
							pageToReplace = pageFrames[j];
						}
					}

					// 从内存中移除被选中的页面，表示该页面被替换
					pageFrames.erase(std::remove(pageFrames.begin(), pageFrames.end(), pageToReplace), pageFrames.end());
				}

				// 将当前页面加入内存
				pageFrames.push_back(page);
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}

	// Clock算法实现
	double StorageManage::Clock(const std::vector<int>& executedPages, int numFrames) {
		std::vector<int> pageFrames(numFrames, -1);			// 初始化页面框架，所有页面初始为 -1，表示空
		std::vector<bool> referenceBit(numFrames, false);	// referenceBit 用来标记页面的访问状态，初始化为 false
		int pageFaults = 0;							// 记录页面缺失次数
		int hand = 0;								// 指向当前检查位置的指针，模拟时钟指针

		// 遍历所有访问的页面
		for (int page : executedPages) {
			bool pageFound = false;					// 用来标记当前页面是否已经在内存中

			// 检查页面是否已经在内存中
			for (int i = 0; i < numFrames; ++i) {
				// 如果页面已经在内存中，更新对应的 referenceBit 标记为 true，并跳出循环
				if (pageFrames[i] == page) {
					referenceBit[i] = true;			// 将页面标记为已被访问
					pageFound = true;				// 页面已找到，不需要替换
					break;
				}
			}

			// 如果页面没有找到（即页面缺失）
			if (!pageFound) {
				pageFaults++;						// 页面缺失次数加1

				// 使用Clock算法选择一个页面进行替换
				while (referenceBit[hand]) {		// 如果当前指针所指的页面已经被访问过
					referenceBit[hand] = false;		// 重置该页面的访问标记
					hand = (hand + 1) % numFrames;  // 顺时针移动时钟指针，循环遍历所有页面框架
				}

				// 选中当前时钟指针指向的页面框架进行替换
				pageFrames[hand] = page;			// 将当前页面加载到该页面框架
				referenceBit[hand] = true;			// 标记该页面已被访问
				hand = (hand + 1) % numFrames;		// 移动时钟指针，指向下一个页面框架
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}


}