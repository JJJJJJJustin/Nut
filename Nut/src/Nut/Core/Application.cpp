#include "nutpch.h"

#include <GLFW/glfw3.h>

#include "Nut/Core/Application.h"

#include "Nut/Events/Event.h"
#include "Nut/Core/Log.h"
#include "Nut/Core/Input.h"

#include "Nut/Renderer/Renderer.h"

namespace Nut {

	Application* Application::s_Instance = nullptr;										//! ! !��ʼ��Ψһʵ���ľ�̬��Աs_Instance

	Application::Application()
	{
		NUT_PROFILE_FUNCTION();

		NUT_CORE_ASSERT(!s_Instance, "Application already exists! (The class Application is a Singleton, it just support one instance!)");
		s_Instance = this;																//! ! !��Ψһʵ���ľ�̬��Ա�Ķ���

		m_Window = Window::Create();							//����������Create�б���ʼ���������m_Window��WindowsWindow.h�е�m_Window����ͬһ��
		m_Window->SetVSync(true);
		m_Window->SetEventCallback(NUT_BIND_EVENT_FN(Application::OnEvent));
		
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();												//��ʼ�� m_ImGuiLayer Ϊԭʼָ�룬�������ջ
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

	void Application::OnEvent(Event& e)																		// --- �� Application �Ĺ��캯���б����ã������¼��ַ���
	{
		NUT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NUT_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(NUT_BIND_EVENT_FN(Application::OnWindowResize));

		//NUT_CORE_TRACE("{0}", e);

		for (auto iter = m_LayerStack.rbegin(); iter != m_LayerStack.rend(); ++iter)	//ͼ����¼������Ƿ���ģ���β��ͷ��������������������е� iter ��Ҫʹ��ǰ�õ������ȼӺ��ã�
		{
			(*iter)->OnEvent(e);														//�����һ����������ָ��Ԫ�ؿ�ʼ�����������Ӧ�¼�
			if (e.Handled)																//�����OnEvent�гɹ����д�����Handled��Ϊtrue��������ѭ��
			{
				break;
			}
		}
	}

	void Application::Run()																					// --- �� ��ڵ� �б�ʹ�ã���Ϊ��Ⱦѭ����
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

					for (Layer* layer : m_LayerStack)				//����ͼ��
						layer->OnUpdate(timestep);					//ִ��ͼ���߼�����(����Ӧ�ó�����߼�״̬��
				}

				m_ImGuiLayer->Begin(); 
				{
					NUT_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();						//����ͼ��ʵ����Ⱦ�������߼����º���ܽ��е���Ⱦ������
				}
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();								//���´���
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