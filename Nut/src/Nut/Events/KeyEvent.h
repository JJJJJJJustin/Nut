#pragma once

#include"Event.h"

namespace Nut {
	class NUT_API KeyEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)				//事件：输入事件  输入设备：键盘（输入设备可以不止是键盘，鼠标和触摸屏的输入都算输入）

		inline int GetKeyCode() const { return m_KeyCode; }
	protected:																			//！！！继承类只能访问受保护的和公有的，不能访问私有的（友元则是私有和受保护的）
		int	m_KeyCode;

		KeyEvent(int keycode)
			:m_KeyCode(keycode) {}
	};


		class NUT_API KeyPressedEvent : public KeyEvent
		{
		public:
			KeyPressedEvent(int keycode, int repeatCount)
				:KeyEvent(keycode), m_RepeatCount(repeatCount) {}							//可以在构造参数列表中使用函数KeyEvent()

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