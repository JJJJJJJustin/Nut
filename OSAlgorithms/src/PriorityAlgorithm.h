#pragma once

namespace Nut
{

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

}