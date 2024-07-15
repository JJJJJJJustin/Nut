#include "nutpch.h"

#include <GLFW/glfw3.h>

#include "Nut/Core/Application.h"

#include "Nut/Events/Event.h"
#include "Nut/Core/Log.h"
#include "Nut/Core/Input.h"

#include "Nut/Renderer/Renderer.h"

namespace Nut {

	Application* Application::s_Instance = nullptr;										//! ! !初始化唯一实例的静态成员s_Instance

	Application::Application()
	{
		NUT_PROFILE_FUNCTION();

		NUT_CORE_ASSERT(!s_Instance, "Application already exists! (The class Application is a Singleton, it just support one instance!)");
		s_Instance = this;																//! ! !对唯一实例的静态成员的定义

		m_Window = Window::Create();							//（上下文在Create中被初始化）这里的m_Window和WindowsWindow.h中的m_Window不是同一个
		m_Window->SetVSync(true);
		m_Window->SetEventCallback(NUT_BIND_EVENT_FN(Application::OnEvent));
		
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();												//初始化 m_ImGuiLayer 为原始指针，并推入层栈
		PushOverlay(m_ImGuiLayer);
	}
	
	Application::~Application()
	{
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		NUT_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();																					// OnAttach is declared in Layer and defined ImGuiLayer
	}

	void Application::PushOverlay(Layer* overlay)
	{
		NUT_PROFILE_FUNCTION();

		m_LayerStack.PushOverLay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)																		// --- 在 Application 的构造函数中被调用（用作事件分发）
	{
		NUT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NUT_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(NUT_BIND_EVENT_FN(Application::OnWindowResize));

		//NUT_CORE_TRACE("{0}", e);

		for (auto iter = m_LayerStack.rbegin(); iter != m_LayerStack.rend(); ++iter)	//图层的事件处理是反向的（从尾到头），！！！反向迭代器中的 iter 需要使用前置递增（先加后用）
		{
			(*iter)->OnEvent(e);														//从最后一个迭代器所指的元素开始，逐个逆向相应事件
			if (e.Handled)																//如果在OnEvent中成功进行处理并将Handled变为true，则跳出循环
			{
				break;
			}
		}
	}

	void Application::Run()																					// --- 在 入口点 中被使用（作为渲染循环）
	{
		NUT_PROFILE_FUNCTION();

		while (m_Running)
		{
			NUT_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) 
			{
				{
					NUT_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)				//更新图层
						layer->OnUpdate(timestep);					//执行图层逻辑更新(更新应用程序的逻辑状态）
				}

				m_ImGuiLayer->Begin(); 
				{
					NUT_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();						//进行图层实际渲染操作（逻辑更新后才能进行的渲染操作）
				}
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();								//更新窗口
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		NUT_PROFILE_FUNCTION();

		m_Minimized = false;
		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());
		return false;

		if(event.GetWidth() == 0 && event.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
	}


}