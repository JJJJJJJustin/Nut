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

	class Banker {
	public:
		Banker(int n, int m);
		void requestResources(int processId, std::vector<int> request);
		void releaseResources(int processId, std::vector<int> release);
		void printStatus();

	private:
		int n; // ������
		int m; // ��Դ����
		std::vector<std::vector<int>> allocation; // �������
		std::vector<std::vector<int>> max;        // ����������
		std::vector<std::vector<int>> need;       // �������
		std::vector<int> available;          // ������Դ
		bool isSafe();

		void calculateNeed();
	};

	Banker::Banker(int numProcesses, int numResources) : n(numProcesses), m(numResources) {
		allocation.resize(n, std::vector<int>(m));
		max.resize(n, std::vector<int>(m));
		need.resize(n, std::vector<int>(m));
		available.resize(m);

		srand(time(0));

		// ������ɿ�����Դ
		for (int j = 0; j < m; j++) {
			available[j] = rand() % 20 + 1;     // ������Դ��Χ 1~20
		}

		// ������ɷ�����������������
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				allocation[i][j] = rand() % 20; // ��Դ���䷶Χ 0~19
				max[i][j] = allocation[i][j] + (rand() % (20 - allocation[i][j])); // �������֤������
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
		for (int j = 0; j < m; j++) {
			if (request[j] > need[processId][j] || request[j] > available[j]) {
				std::cout << "���� " << processId << " ��������Ч��" << std::endl;
				return;
			}
		}

		// ��ʱ������Դ
		for (int j = 0; j < m; j++) {
			available[j] -= request[j];
			allocation[processId][j] += request[j];
			need[processId][j] -= request[j];
		}

		// ���ϵͳ�Ƿ�ȫ
		if (!isSafe()) {
			// �������ȫ���ع�
			for (int j = 0; j < m; j++) {
				available[j] += request[j];
				allocation[processId][j] -= request[j];
				need[processId][j] += request[j];
			}
			std::cout << "���� " << processId << " �������²���ȫ���ع���" << std::endl;
		}
	}

	void Banker::releaseResources(int processId, std::vector<int> release) {
		for (int j = 0; j < m; j++) {
			allocation[processId][j] -= release[j];
			available[j] += release[j];
		}
		calculateNeed();
		std::cout << "���� " << processId << " �ͷ���Դ�ɹ���" << std::endl;
	}

	void Banker::printStatus() {
		std::cout << "��Դ����״��:" << std::endl;
		std::cout << "����\t����\t\t�������\t����" << std::endl;
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

		std::cout << "������Դ: ";
		for (int j = 0; j < m; j++) {
			std::cout << available[j] << " ";
		}
		std::cout << std::endl;
	}



}