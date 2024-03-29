#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"

#include "Nut/Events/ApplicationEvent.h"

namespace Nut {

	class NUT_API Application							//类的导出
	{
	public:
		Application();
		virtual ~Application();							//支持多态

		void Run();

		void OnEvent(Event& e);							//事件分发

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }			//返回下面这个指向Window的指针
		inline static Application& Get() { return *s_Instance; }	//! ! !返回的是s_Instance这个指向Application的指针
																	//（为什么函数是引用传递？：因为application是一个单例，如果不使用&则会多出一个复制，这有悖于单例模式的只有一个对象的要求
	private:
		bool OnWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;				//指向Window的指针
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;					//! ! !唯一实例的静态成员（static类型，需要初始化定义）
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}