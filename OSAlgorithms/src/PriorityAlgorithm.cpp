#include "nutpch.h"
#include "PriorityAlgorithm.h"

#include "Nut/Scene/Scene.h"

#include <ctime>
#include <cstdlib>
#include <iomanip>

#include <yaml-cpp/yaml.h>

namespace Nut
{
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
        std::vector<Process> completed_processes; // ��ɵĽ���
        int time = 0; // ��ǰʱ��

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




}