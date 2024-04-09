#include "nutpch.h"
#include "Application.h"

#include "Events/Event.h"
#include "Nut/Log.h"
#include "Input.h"

#include <glad/glad.h>

namespace Nut {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)		//��һ����Ա��������ռλ�������Ժ�ʹ��  !!!��_1������ 1,�궨�����Ҫд;!!!

	Application* Application::s_Instance = nullptr;										//! ! !��ʼ��Ψһʵ���ľ�̬��Աs_Instance

	Application::Application()
	{
		NUT_CORE_ASSERT(!s_Instance, "Application already exists! (The class Application is a Singleton, it just support one instance!)");
		s_Instance = this;																//! ! !��Ψһʵ���ľ�̬��Ա�Ķ���

		m_Window = std::unique_ptr<Window>(Window::Create());							//�����m_Window��WindowsWindow.h�е�m_Window����ͬһ��
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();												//��ʼ�� m_ImGuiLayer Ϊԭʼָ�룬�������ջ
		PushOverlay(m_ImGuiLayer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER ,m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glEnableVertexAttribArray(0);													//��������Ϊ0���������ݲ�������Ϊ��������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);	//������Ϊ0�����ݽ��ж�����������

		unsigned int indices[3]{
			0, 1, 2
		};

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;

			void main()
			{
				gl_Position = vec4(a_Position, 1.0);
				v_Position = a_Position;
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 a_Color;
			in vec3 v_Position;
			
			void main()
			{
				a_Color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)				//����ͼ��
				layer->OnUpdate();							//ִ���߼�����(����Ӧ�ó�����߼�״̬��
			
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