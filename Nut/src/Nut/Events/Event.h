#pragma once

#include "Nut\Core.h"

#include <string>
#include <functional>

namespace Nut {
	/*I hear that from the Cherno :
	  The events we set in Nut now are currently blocking.
	  When event occurs,it immediately gets dispatched���õ����ȣ� and must be dealt there right now.
	  we might be to buffer events in a event bus (�¼�����) ,so we can process them during event part stage.
	*/

	//ENUM----------------------------------------------------------------------------
	enum class EventType {								//�¼�����
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
	};
	//enum classʹ��ʱ�����������ƣ���Ҫ EventType:: ���á�enum���á�

	enum EventCategory {								//�¼����
		None = 0,
		EventCategoryApplication = Bit(0),
		EventCategoryInput = Bit(1),
		EventCategoryKeyboard = Bit(2),
		EventCategoryMouse = Bit(3),
		EventCategoryMouseButton = Bit(4)
	};

	//�궨�壨�ı��滻���Զ������µ���д����---------------------------------------------------
#define EVENT_CLASS_TYPE(type) \
			static EventType GetStaticType(){ return EventType::##type; }\
			virtual EventType GetEventType() const override { return GetStaticType(); }\
			virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)\
			virtual int GetCategoryFlags() const override { return category; }

//CLASS--------------------------------------------------------------------------------
	class NUT_API Event
	{
		friend class EventDispatcher;										//������ԪEventDispatcher
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }			//��������Ϊ��Ա����ʹ�ã�event.ToString()�����ַ�����

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;							//�¼���ͬʱ�� 00000001 & 00000001 ������ 00000001 ����¼�
																			//�¼�����ͬʱ 00000001 & 00000010 ����� 00000011 �����¼����ܺͣ���������ƶ�����갴ťͬʱ������
		}
	protected:
		bool m_Handled = false;												//�Ƿ��Ѿ�����Handled����Ϊ��Ԫ�ṩ����
	};


	class EventDispatcher
	{
	private:
		Event& m_Event;

		template<typename T>												//ΪEventFn�������������Ҫ��ģ��T
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			:m_Event(event)
		{
		}
		template<typename T>												//ΪDispatch�����ṩ��Ҫʹ�õ�ģ��T����������ֻдһ��template������EventFn��˽�еģ��͹��е����ֿ���
		bool Dispatch(EventFn<T> func) {									//����һ��������ΪT���ҡ�����ֵΪbool���ĺ���func
			if (m_Event.GetEventType() == T::GetStaticType())				//��������̬������ʹ��ʱ��Ҫʹ��������������������
			{													//����������Ĳ�����Ϊ�˸�ʲô,T����ɶ����������ʹ��
				m_Event.m_Handled = func(*(T*)&m_Event);					//*(T*) ��ʾ���� * �����ã�T*����������T����ָ�룬ʵ��ǿ������ת��
				return true;
			}
			return false;
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& ev)
	{
		return os << ev.ToString();
	}
}