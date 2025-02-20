#include "nutpch.h"
#include "Nut/Core/UUID.h"

#include <random>
//#include <unordered_map>

//static std::unordered_map<Nut::UUID, std::string> s_Map;

namespace Nut
{
	static std::random_device s_RD;						// 设置随机数种子，用于初始化引擎
	static std::mt19937_64 s_Engine(s_RD());				// 根据随机数种子，创建随机数生成器（随机数引擎）
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;		// 创建分布对象（用于约束随机数生成的范围、频率等特征）

	UUID::UUID()
		:m_UUID(s_UniformDistribution(s_Engine))		// 使用随机数引擎生成一个数字，并使用分布对象将其转化
	{
	}

	UUID::UUID(uint64_t uuid)
		:m_UUID(uuid)
	{
	}

}