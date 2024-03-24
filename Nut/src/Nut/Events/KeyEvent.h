#pragma once

#include"Event.h"

namespace Nut {
	class NUT_API KeyEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)				//�¼��������¼�  �����豸�����̣������豸���Բ�ֹ�Ǽ��̣����ʹ����������붼�����룩

		inline int GetKeyCode() const { return m_KeyCode; }
	protected:																			//�������̳���ֻ�ܷ����ܱ����ĺ͹��еģ����ܷ���˽�еģ���Ԫ����˽�к��ܱ����ģ�
		int	m_KeyCode;

		KeyEvent(int keycode)
			:m_KeyCode(keycode) {}
	};


		class NUT_API KeyPressedEvent : public KeyEvent
		{
		public:
			KeyPressedEvent(int keycode, int repeatCount)
				:KeyEvent(keycode), m_RepeatCount(repeatCount) {}							//�����ڹ�������б���ʹ�ú���KeyEvent()

			EVENT_CLASS_TYPE(KeyPressed)

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << m_KeyCode << "(" << m_RepeatCount << ")";
				return ss.str();
			}

			inline int GetRepeatCount() const { return m_RepeatCount; }
		private:
			int m_RepeatCount;
		};


		class NUT_API KeyReleasedEvent : public KeyEvent
		{
		public:
			KeyReleasedEvent(int keycode)
				:KeyEvent(keycode) {}

			EVENT_CLASS_TYPE(KeyReleased)

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyReleasedEvent: " << m_KeyCode;
				return ss.str();
			}
		};

		class NUT_API KeyTypedEvent : public KeyEvent
		{
		public:
			KeyTypedEvent(int keycode)
				:KeyEvent(keycode) {}

			EVENT_CLASS_TYPE(KeyTyped)

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyTypedEvent: " << m_KeyCode;
				return ss.str();
			}
		};
}