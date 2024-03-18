#include "nutpch.h"
#include "Application.h"

#include "Events/Event.h"
#include "Nut/Log.h"

#include <GLFW/glfw3.h>

namespace Nut {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)		//��һ����Ա��������ռλ�������Ժ�ʹ��  !!!��_1������ 1,�궨�����Ҫд;!!!

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());							//�����m_Window��WindowsWindow.h�е�m_Window����ͬһ��
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{

	}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverLay(overlay);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		NUT_CORE_TRACE("{0}", e);

		for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin(); )				//ͼ����¼������Ƿ���ģ���β��ͷ��
		{
			(*--iter)->OnEvent(e);														//�����һ����������ָ��Ԫ�ؿ�ʼ�����������Ӧ�¼�
			if (e.Handled)																//�����OnEvent�гɹ����д�����Handled��Ϊtrue��������ѭ��
			{
				break;
			}
		}
	}

	void Application::Run()
	{
		WindowResizeEvent WRE(1280, 720);
		if (WRE.IsInCategory(EventCategoryApplication))
		{
			NUT_TRACE(WRE);
		}
		if (WRE.IsInCategory(EventCategoryMouseButton))
		{
			NUT_TRACE(WRE);
		}
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)				//����ͼ��
			{
				layer->OnUpdate();
			}
			m_Window->OnUpdate();							//���´���
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}