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
        std::vector<Process> completed_processes; // 完成的进程
        int time = 0; // 当前时间

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




}