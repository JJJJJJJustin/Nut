#include "nutpch.h"
#include "Nut/Core/UUID.h"

#include <random>
//#include <unordered_map>

//static std::unordered_map<Nut::UUID, std::string> s_Map;

namespace Nut
{
	static std::random_device s_RD;						// ������������ӣ����ڳ�ʼ������
	static std::mt19937_64 s_Engine(s_RD());				// ������������ӣ��������������������������棩
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;		// �����ֲ���������Լ����������ɵķ�Χ��Ƶ�ʵ�������

	UUID::UUID()
		:m_UUID(s_UniformDistribution(s_Engine))		// ʹ���������������һ�����֣���ʹ�÷ֲ�������ת��
	{
	}

	UUID::UUID(uint64_t uuid)
		:m_UUID(uuid)
	{
	}

}