#pragma once

#include "nutpch.h"

#include "Nut\Core\Base.h"

namespace Nut {
	/*I hear that from the Cherno :
	  The events we set in Nut now are currently blocking.
	  When event occurs,it immediately gets dispatched（得到调度） and must be dealt there right now.
	  we might be to buffer events in a event bus (事件总线) ,so we can process them during event part stage.
	*/

	//ENUM----------------------------------------------------------------------------
	enum class EventType {								//事件类型
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
	};
	//enum class使用时有作用域限制，需要 EventType:: 引用。enum不用。

	enum EventCategory {								//事件类别
		None = 0,
		EventCategoryApplication = Bit(0),
		EventCategoryInput = Bit(1),
		EventCategoryKeyboard = Bit(2),
		EventCategoryMouse = Bit(3),
		EventCategoryMouseButton = Bit(4)
	};

	//宏定义（文本替换）自动化以下的重写操作---------------------------------------------------
#define EVENT_CLASS_TYPE(type) \
			static EventType GetStaticType(){ return EventType::type; }\
			virtual EventType GetEventType() const override { return GetStaticType(); }\
			virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)\
			virtual int GetCategoryFlags() const override { return category; }

	//CLASS--------------------------------------------------------------------------------
	class Event
	{
		friend class EventDispatcher;										//声明友元EventDispatcher
	public:
		bool Handled = false;												//是否已经处理（Handled）


		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }			//！！！作为成员函数使用：event.ToString()返回字符串。

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;							//事件相同时候 00000001 & 00000001 依旧是 00000001 这个事件
																			//事件不相同时 00000001 & 00000010 结果是 00000011 两个事件的总和（例如鼠标移动和鼠标按钮同时触发）
		}
	};


	class EventDispatcher
	{
	private:
		Event& m_Event;

		template<typename T>												//为EventFn这个别名声明需要的模板T
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			:m_Event(event)
		{
		}
		template<typename T>												//为Dispatch函数提供需要使用的模板T（本来可以只写一个template，但是EventFn是私有的，和公有的区分开）
		bool Dispatch(EventFn<T> func) {									//接受一个“参数为T”且“返回值为bool”的函数func（这里将会在未来使用中填入一个函数指针）
			if (m_Event.GetEventType() == T::GetStaticType())				//！！！静态函数在使用时需要使用类名或类型名来调用
			{									//？？？if语句中的判断是为了干什么,T会是啥，会怎样被使用：typename T 在使用时作为允许标识，当接受一个未知类型的事件时，
												//我们会判断该事件与被标识允许执行自定义操作的事件之间类型是否相同（操作即执行填入的函数func），若相同则执行我们定义的事件
				m_Event.Handled = func( static_cast<T&>(m_Event) );						//*(T*) 表示：用 * 解引用（T*）所声明的T类型指针，实现强制类型转换（在这里我们使用了func并将event作为参数传入，以此执行函数操作，并返回该函数的返回值赋值给Handled）
				return true;												// 这个 return 用于结束该函数（ Dispatch 函数），和 Handled 没什么关系， Handled 是 func 返回的值
			}
			return false;
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& ev)
	{
		return os << ev.ToString();
	}
}