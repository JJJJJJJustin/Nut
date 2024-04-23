#include "nutpch.h"

#include <GLFW/glfw3.h>

#include "Application.h"

#include "Events/Event.h"
#include "Nut/Log.h"
#include "Input.h"

#include "Nut/Renderer/Renderer.h"

namespace Nut {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)		//��һ����Ա��������ռλ�������Ժ�ʹ��  !!!��_1������ 1,�궨�����Ҫд;!!!

	Application* Application::s_Instance = nullptr;										//! ! !��ʼ��Ψһʵ���ľ�̬��Աs_Instance

	Application::Application()
	{
		NUT_CORE_ASSERT(!s_Instance, "Application already exists! (The class Application is a Singleton, it just support one instance!)");
		s_Instance = this;																//! ! !��Ψһʵ���ľ�̬��Ա�Ķ���

		m_Window = std::unique_ptr<Window>(Window::Create());							//����������Create�б���ʼ���������m_Window��WindowsWindow.h�е�m_Window����ͬһ��
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_Window->SetVSync(true);

		m_ImGuiLayer = new ImGuiLayer();												//��ʼ�� m_ImGuiLayer Ϊԭʼָ�룬�������ջ
		PushOverlay(m_ImGuiLayer);
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

	void Application::OnEvent(Event& e)																		// --- �� Application �Ĺ��캯���б����ã������¼��ַ���
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//NUT_CORE_TRACE("{0}", e);

		for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin(); )			//ͼ����¼������Ƿ���ģ���β��ͷ��
		{
			(*--iter)->OnEvent(e);														//�����һ����������ָ��Ԫ�ؿ�ʼ�����������Ӧ�¼�
			if (e.Handled)																//�����OnEvent�гɹ����д�����Handled��Ϊtrue��������ѭ��
			{
				break;
			}
		}
	}

	void Application::Run()																					// --- �� ��ڵ� �б�ʹ�ã���Ϊ��Ⱦѭ����
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)				//����ͼ��
				layer->OnUpdate(timestep);					//ִ���߼�����(����Ӧ�ó�����߼�״̬��
			
			//auto [x, y] = Input::GetMousePos();
			//NUT_CORE_TRACE("{0},{1}", x, y);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();						// ����ʵ����Ⱦ�������߼����º���ܽ��е���Ⱦ������
			m_ImGuiLayer->End();

			m_Window->OnUpdate();							//���´���
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}