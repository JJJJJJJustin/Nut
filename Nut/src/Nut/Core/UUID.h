#pragma once

#include <xhash>		//  ?  why xhash file  ?

namespace Nut 
{

	class UUID
	{
	public:
		UUID();// Ϊ UUID ���Ͷ����ϣ����
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std
{
	// // Ϊ UUID ���Ͷ����ϣ������ȷ�� UUID ���Ա���ϣ����
	template<>
		struct hash<Nut::UUID>
	{
		std::size_t operator()(const Nut::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}