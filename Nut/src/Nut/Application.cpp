#include "nutpch.h"
#include "Application.h"

#include "Events/Event.h"
#include "Nut/Log.h"
#include "Input.h"

#include <glad/glad.h>

namespace Nut {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)		//绑定一个成员函数，并占位参数，稍后使用  !!!是_1而不是 1,宏定义最后不要写;!!!

	Application* Application::s_Instance = nullptr;										//! ! !初始化唯一实例的静态成员s_Instance

	Application::Application()
	{
		NUT_CORE_ASSERT(!s_Instance, "Application already exists! (The class Application is a Singleton, it just support one instance!)");
		s_Instance = this;																//! ! !对唯一实例的静态成员的定义

		m_Window = std::unique_ptr<Window>(Window::Create());							//这里的m_Window和WindowsWindow.h中的m_Window不是同一个
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();												//初始化 m_ImGuiLayer 为原始指针，并推入层栈
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverLay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)																		// --- 在 Application 的构造函数中被调用（用作事件分发）
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//NUT_CORE_TRACE("{0}", e);

		for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin(); )			//图层的事件处理是反向的（从尾到头）
		{
			(*--iter)->OnEvent(e);														//从最后一个迭代器所指的元素开始，逐个逆向相应事件
			if (e.Handled)																//如果在OnEvent中成功进行处理并将Handled变为true，则跳出循环
			{
				break;
			}
		}
	}

	void Application::Run()																					// --- 在 入口点 中被使用（作为渲染循环）
	{
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)				//更新图层
				layer->OnUpdate();							// ? ? ?执行逻辑更新(什么作用？）
			
			//auto [x, y] = Input::GetMousePos();
			//NUT_CORE_TRACE("{0},{1}", x, y);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();						// ? ? ?进行渲染操作（和上面有什么不同？）
			m_ImGuiLayer->End();

			m_Window->OnUpdate();							//更新窗口
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}