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
			// �����ض��Ĺؼ���
			int processPos = line.find("Running process: ");
			int timePos = line.find("executed for ");

			if (processPos != std::string::npos && timePos != std::string::npos) {
				// ��ȡ��������
				int start = processPos + 17; // "Running process: " �ĳ���
				int end = line.find("(", start);
				std::string processName = line.substr(start, end - start - 1);

				// ��ȡִ��ʱ��
				int startTime = timePos + 13; // "executed for " �ĳ���
				int endTime = line.find("s", startTime);
				std::string executeTimeStr = line.substr(startTime, endTime - start);
				int executionTime = std::stoi(executeTimeStr);

				// ��ӽ���� vector
				results.emplace_back(processName, executionTime);
			}
		}
	}
	else {
		NUT_CORE_ASSERT(file.is_open(), "Could not open file !");
	}

	return results; // ���������ҵ��Ľ��
}

namespace Nut
{
	// -----------------------------------------------------------------------------------
	// ---------------------------- Priority Algorithm -----------------------------------
	// -----------------------------------------------------------------------------------
	bool PriorityAlgorithm::compare(const Process& a, const Process& b)
	{
		return a.priority > b.priority; // ���ȼ��ߵ�����ǰ��
	}

	void PriorityAlgorithm::generate_processes(std::vector<Process>& processes, int num_processes)
	{
		for (int i = 0; i < num_processes; ++i) {
			std::string name = "P" + std::to_string(i + 1);
			int priority = rand() % 10 + 1;       // ���ȼ���Χ 1-10
			int arrival_time = rand() % 10;       // ����ʱ�䷶Χ 0-9
			int service_time = rand() % 20 + 1;   // ����ʱ�䷶Χ 1-20
			processes.emplace_back(name, priority, arrival_time, service_time);
		}
	}

	std::vector<Process> PriorityAlgorithm::priority_scheduling(std::vector<Process>& processes)
	{
		std::vector<Process> completed_processes;	// ��ɵĽ���
		int time = 0;								// ��ǰʱ��

		while (!processes.empty()) {
			// ���ѵ���Ľ�����ӵ���������
			std::vector<Process*> ready_queue;
			for (auto& process : processes) {
				if (process.arrival_time <= time) {
					ready_queue.push_back(&process);
				}
			}

			// ����������в�Ϊ��
			if (!ready_queue.empty()) {
				// �������ȼ�����
				std::sort(ready_queue.begin(), ready_queue.end(),
					[](Process* a, Process* b) { return a->priority > b->priority; });

				// ѡ�����ȼ���ߵĽ���
				Process* current_process = ready_queue.front();
				current_process->remaining_time--;

				std::cout << "Time: " << time << " - Running process: " << current_process->name << std::endl;

				// �����ǰ�������
				if (current_process->remaining_time == 0) {
					// ������תʱ��
					current_process->turnaround_time = time + 1 - current_process->arrival_time; // +1 ����Ϊʱ���0��ʼ

					completed_processes.push_back(*current_process);
					processes.erase(std::remove_if(processes.begin(), processes.end(),
						[current_process](Process& p) { return p.name == current_process->name; }), processes.end());
				}
			}
			else {
				std::cout << "Time: " << time << " - No process is running." << std::endl;
			}

			time++; // ʱ���ƽ�
		}

		// �����ɵĽ���˳�򼰷���ʱ�����תʱ��
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
	//������ɽ���
	void RoundAlgorithm::generate_processes(std::vector<Process>& processes, int num_processes)
	{
		for (int i = 0; i < num_processes; ++i) {
			std::string name = "P" + std::to_string(i + 1);
			int priority = rand() % 10 + 1;       // ���ȼ���Χ 1-10
			int arrival_time = rand() % 10;       // ����ʱ�䷶Χ 0-9
			int service_time = rand() % 20 + 1;   // ����ʱ�䷶Χ 1-20
			processes.emplace_back(name, priority, arrival_time, service_time);
		}

	}

	// ��ת�����㷨
	std::vector<Process> RoundAlgorithm::round_robin(std::vector<Process>& processes, int time_segment, const std::string& output_filename) {
		std::queue<int> ready_queue; // ��������
		int current_time = 0;
		int n = processes.size();
		int processes_completed = 0;

		// ������̸��ݵ���ʱ��
		std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });

		#// ���ļ������
		std::ofstream output_file(output_filename);
		NUT_CORE_ASSERT(output_file.is_open(), "Failed to open file ! ")

		while (processes_completed < n) {
			// ������Ľ��̼����������
			for (int i = 0; i < n; ++i) {
				if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
					ready_queue.push(i);
				}
			}

			if (!ready_queue.empty()) {
				int current_process_index = ready_queue.front();
				ready_queue.pop();

				// ��鵱ǰ�����Ƿ���Ȼ��Ҫִ��
				if (processes[current_process_index].remaining_time > 0) {
					// �����ǰʱ����������еĽ���
					output_file << "Time: " << current_time << " - Running process: " << processes[current_process_index].name;

					// ����ǰ����
					int execution_time = std::min(processes[current_process_index].remaining_time, time_segment);
					processes[current_process_index].remaining_time -= execution_time;
					current_time += execution_time;

					// �����ǰ����ִ�е�ʱ��
					output_file << " ( executed for " << execution_time << "s";

					if (processes[current_process_index].remaining_time == 0) {
						processes[current_process_index].turnaround_time = current_time - processes[current_process_index].arrival_time;
						output_file << " and finished.\n"; // ��������Ϣ
						processes_completed++;
					}
					else {
						output_file << " and not finished.\n"; // ���δ�����Ϣ
						ready_queue.push(current_process_index);
					}
				}

				// �������δ��ɣ����¼����������
				if (processes[current_process_index].remaining_time > 0) {
					ready_queue.push(current_process_index);
				}
			}
			else {
				// ���û�н��̿�ִ�У���������һ���ɵ������
				current_time++;
			}
		}

		// �ر��ļ������
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

		// ������ɿ�����Դ
		for (int j = 0; j < m_Type; j++) {
			m_Available[j] = rand() % 20 + 1;     // ������Դ��Χ 1~20
		}

		// ������ɷ�����������������
		for (int i = 0; i < m_Num; i++) {
			for (int j = 0; j < m_Type; j++) {
				m_Allocation[i][j] = rand() % 20; // ��Դ���䷶Χ 0~19
				m_Max[i][j] = m_Allocation[i][j] + (rand() % (20 - m_Allocation[i][j])); // �������֤������
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
				return false;       // �Ҳ�����ȫ����
			}
		}

		// �����ȫ����
		std::cout << "��ȫ����: ";
		for (int i : safeSequence) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		return true;
	}

	void Banker::requestResources(int processId, std::vector<int> request) {
		// ��������Ƿ�Ϸ�
		for (int j = 0; j < m_Type; j++) {
			if (request[j] > m_Need[processId][j] || request[j] > m_Available[j]) {
				std::cout << "���� " << processId << " ��������Ч��" << std::endl;
				return;
			}
		}

		// ��ʱ������Դ
		for (int j = 0; j < m_Type; j++) {
			m_Available[j] -= request[j];
			m_Allocation[processId][j] += request[j];
			m_Need[processId][j] -= request[j];
		}

		// ���ϵͳ�Ƿ�ȫ
		if (!isSafe()) {
			// �������ȫ���ع�
			for (int j = 0; j < m_Type; j++) {
				m_Available[j] += request[j];
				m_Allocation[processId][j] -= request[j];
				m_Need[processId][j] += request[j];
			}
			std::cout << "���� " << processId << " �������²���ȫ���ع���" << std::endl;
		}
	}

	void Banker::releaseResources(int processId, std::vector<int> release) {
		for (int j = 0; j < m_Type; j++) {
			m_Allocation[processId][j] -= release[j];
			m_Available[j] += release[j];
		}
		calculateNeed();
		std::cout << "���� " << processId << " �ͷ���Դ�ɹ���" << std::endl;
	}

	void Banker::printStatus() {
		std::cout << "��Դ����״��:" << std::endl;
		std::cout << "����\t����\t\t�������\t����" << std::endl;
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

		std::cout << "������Դ: ";
		for (int j = 0; j < m_Type; j++) {
			std::cout << m_Available[j] << " ";
		}
		std::cout << std::endl;
	}


	// ---------------------------------------------------------------
	// ---------------------- Storage Management ---------------------
	// ---------------------------------------------------------------
	std::vector<int> StorageManage::m_ExecutedPages;					// ���徲̬��Ա����

	void StorageManage::GenerateInstructions(const std::string& filename, int totalInstructions, int maxAddress) {
		// ���ļ�
		std::ofstream file(filename);
		NUT_CORE_ASSERT(file, "�޷����ļ�");

		// �������������
		std::srand(std::time(nullptr));

		// ����ÿ��ָ�������
		int numSequential = totalInstructions / 2;
		int numFront = totalInstructions / 4;
		int numBack = totalInstructions / 4;

		// ����˳��ִ�е�ָ��
		for (int i = 0; i < numSequential; ++i) {
			int address = i;  // ˳���ַ
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// ���ɾ��ȷֲ���ǰ��ַ���ֵ�ָ��
		for (int i = 0; i < numFront; ++i) {
			int address = std::rand() % (maxAddress / 2);  // ǰ�벿�ֵĵ�ַ
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// ���ɾ��ȷֲ��ں��ַ���ֵ�ָ��
		for (int i = 0; i < numBack; ++i) {
			int address = std::rand() % (maxAddress / 2) + (maxAddress / 2);  // ��벿�ֵĵ�ַ
			file << "CMD_" << address << ": Do something at address " << address << std::endl;
		}

		// �ر��ļ�
		file.close();
		NUT_CORE_INFO("�ļ�������ɣ�����{0}��ָ�", totalInstructions)
	}

	std::vector<int> StorageManage::ExecuteInstructions(const std::string& filename) {
		// ��ȡָ��
		std::vector<std::string> instructions;
		ParseInstructions(filename, instructions);

		// �������
		m_ExecutedPages.reserve(320); // ִ��320��ָ��

		// ���������
		std::srand(std::time(nullptr));

		// ���ѡȡһ����ʼ��m��ָ���ַ��
		int m = std::rand() % 320;  // ���ѡȡ��ʼָ���ַ m (��Χ 0 �� 319)

		// ִ��320��ָ��
		for (int i = 0; i < 320; ++i) {
			// ˳��ִ�е�ַΪ m + 1 ��ָ��
			m = (m + 1) % 320;  // ˳��ִ��һ��ָ��� m+1 ��ַ
			m_ExecutedPages.push_back(GetPage(m));

			// ��ǰ��ַ [0, m+1] �����ѡȡһ��ָ��
			int m_prime = std::rand() % (m + 1);  // ���ѡȡһ����ַ
			m_ExecutedPages.push_back(GetPage(m_prime));

			// ˳��ִ�е�ַΪ m' + 1 ��ָ��
			m_prime = (m_prime + 1) % 320;  // ˳��ִ��һ��ָ��
			m_ExecutedPages.push_back(GetPage(m_prime));

			// �ں��ַ [m'+2, 319] �����ѡȡһ��ָ��
			int m_double_prime = std::rand() % (320 - (m_prime + 2)) + (m_prime + 2);
			m_ExecutedPages.push_back(GetPage(m_double_prime));

			// ����m
			m = m_double_prime;
		}

		// ��ӡҳ����
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
		// ÿҳ10��ָ��
		return instructionIndex / 10;
	}


	// --------------- 4 Algorithm ------------------------
	 // FIFO�㷨ʵ��
	double StorageManage::FIFO(const std::vector<int>& executedPages, int numFrames) {
		std::deque<int> pageFrames;								// ˫�˶��У��ڴ��д洢��ҳ�棬���ױ�ʾ���类���ʵ�ҳ�棬��β��ʾ������ʵ�ҳ��
		std::unordered_map<int, bool> pageSet;
		int pageFaults = 0;

		for (int page : executedPages) {
			if (pageSet.find(page) == pageSet.end()) {
				pageFaults++;
				if (pageFrames.size() == numFrames) {			//	���ڴ����������滻���类���ʵ�ҳ��
					int oldest = pageFrames.front();
					pageFrames.pop_front();
					pageSet.erase(oldest);
				}
				pageFrames.push_back(page);						// ����ǰҳ����ӵ� pageFrames �Ķ�β����ʾ��ҳ����������ʵ�ҳ��
				pageSet[page] = true;							// ��Ǹ�ҳ���ѱ�����
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}

	// LRU�㷨ʵ��
	double StorageManage::LRU(const std::vector<int>& executedPages, int numFrames) {
		std::unordered_map<int, std::list<int>::iterator> pageMap;		// ������б��е�ҳ�棬�洢ҳ��������˫�������е�λ��ӳ��
		std::list<int> pageFrames;										// ˫����������ά��ҳ��ķ���˳������ͷΪ������ʵ�ҳ�棩
		int pageFaults = 0;												// ����ȱҳ��ҳ����

		for (int page : executedPages) {
			if (pageMap.find(page) == pageMap.end()) {
				pageFaults++;
				if (pageFrames.size() == numFrames) {					// ����ڴ���������Ҫ�滻ҳ��
					int oldest = pageFrames.back();
					pageFrames.pop_back();								// ���������Ƴ����δ���ʵ�ҳ��
					pageMap.erase(oldest);								// �������ϣ����ɾ����ҳ��
				}
			}
			else {
				pageFrames.erase(pageMap[page]);						// ҳ���Ѿ����ڴ��У��ȴ��������Ƴ���ҳ��
			}

			pageFrames.push_front(page);								// ����ǰҳ���Ƶ������ͷ������ʾ�������±����ʵ�
			pageMap[page] = pageFrames.begin();							// ����ҳ��ӳ�䣬ָ������ͷ��
		}

		return 1.0 - (double)pageFaults / executedPages.size();			// 1 - ��ȱҳ��/ҳ����������
	}

	// OPT�㷨ʵ��
	double StorageManage::OPT(const std::vector<int>& executedPages, int numFrames) {
		std::vector<int> pageFrames;									// pageFrames �����洢��ǰ���ڴ��е�ҳ�棨������У�

		// pageFaults ��¼ҳ��ȱʧ����
		int pageFaults = 0;

		// �������з��ʵ�ҳ��
		for (int i = 0; i < executedPages.size(); ++i) {
			int page = executedPages[i];

			// �����ǰҳ�治���ڴ��У�����ҳ��ȱʧ
			if (std::find(pageFrames.begin(), pageFrames.end(), page) == pageFrames.end()) {
				pageFaults++;

				// ����ڴ�������ѡ��һ��ҳ������滻
				if (pageFrames.size() == numFrames) {
					int farthest = -1;					// ��¼��ǰҳ����δ��������������Զ�ķ���λ��
					int pageToReplace = -1;				// ��¼��Ҫ�滻��ҳ��

					// ���������Ѽ��ص�ҳ�棬�ҳ����δʹ�õ�ҳ��
					for (int j = 0; j < pageFrames.size(); ++j) {
						int k;
						// Ѱ�Ҹ�ҳ����֮��ķ��������г��ֵ���һ��λ��
						for (k = i + 1; k < executedPages.size(); ++k) {
							if (executedPages[k] == pageFrames[j]) {
								// �ҵ��˸�ҳ�棬�����ڲ�ѭ��
								break;
							}
						}

						// �����ҳ��֮����Ҳ���ᱻ���ʣ��������滻��ҳ��
						if (k == executedPages.size()) {
							pageToReplace = pageFrames[j];
							break;
						}

						// �����ҳ��֮�󻹻ᱻ���ʣ���¼�����´η���λ��
						if (k > farthest) {
							farthest = k;
							pageToReplace = pageFrames[j];
						}
					}

					// ���ڴ����Ƴ���ѡ�е�ҳ�棬��ʾ��ҳ�汻�滻
					pageFrames.erase(std::remove(pageFrames.begin(), pageFrames.end(), pageToReplace), pageFrames.end());
				}

				// ����ǰҳ������ڴ�
				pageFrames.push_back(page);
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}

	// Clock�㷨ʵ��
	double StorageManage::Clock(const std::vector<int>& executedPages, int numFrames) {
		std::vector<int> pageFrames(numFrames, -1);			// ��ʼ��ҳ���ܣ�����ҳ���ʼΪ -1����ʾ��
		std::vector<bool> referenceBit(numFrames, false);	// referenceBit �������ҳ��ķ���״̬����ʼ��Ϊ false
		int pageFaults = 0;							// ��¼ҳ��ȱʧ����
		int hand = 0;								// ָ��ǰ���λ�õ�ָ�룬ģ��ʱ��ָ��

		// �������з��ʵ�ҳ��
		for (int page : executedPages) {
			bool pageFound = false;					// ������ǵ�ǰҳ���Ƿ��Ѿ����ڴ���

			// ���ҳ���Ƿ��Ѿ����ڴ���
			for (int i = 0; i < numFrames; ++i) {
				// ���ҳ���Ѿ����ڴ��У����¶�Ӧ�� referenceBit ���Ϊ true��������ѭ��
				if (pageFrames[i] == page) {
					referenceBit[i] = true;			// ��ҳ����Ϊ�ѱ�����
					pageFound = true;				// ҳ�����ҵ�������Ҫ�滻
					break;
				}
			}

			// ���ҳ��û���ҵ�����ҳ��ȱʧ��
			if (!pageFound) {
				pageFaults++;						// ҳ��ȱʧ������1

				// ʹ��Clock�㷨ѡ��һ��ҳ������滻
				while (referenceBit[hand]) {		// �����ǰָ����ָ��ҳ���Ѿ������ʹ�
					referenceBit[hand] = false;		// ���ø�ҳ��ķ��ʱ��
					hand = (hand + 1) % numFrames;  // ˳ʱ���ƶ�ʱ��ָ�룬ѭ����������ҳ����
				}

				// ѡ�е�ǰʱ��ָ��ָ���ҳ���ܽ����滻
				pageFrames[hand] = page;			// ����ǰҳ����ص���ҳ����
				referenceBit[hand] = true;			// ��Ǹ�ҳ���ѱ�����
				hand = (hand + 1) % numFrames;		// �ƶ�ʱ��ָ�룬ָ����һ��ҳ����
			}
		}

		return 1.0 - (double)pageFaults / executedPages.size();
	}


}