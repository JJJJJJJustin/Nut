#pragma once

namespace Nut {

	class Timestep
	{
	public:
		Timestep(float time)
			:m_Time(time)
		{}

		operator float() { return m_Time; }										// Conversion Operators £º×ª»»²Ù×÷·û£¨ This is for a varible which typed by "Timestep" can be use with arithmetic operator like + - * /

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }
	private:
		float m_Time;
	};

}