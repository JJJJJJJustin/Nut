#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"
#include "Nut/ImGui/ImGuiLayer.h"

#include "Nut/Events/ApplicationEvent.h"

#include "Nut/Renderer/Shader.h"

namespace Nut {

	class NUT_API Application							//��ĵ���
	{
	public:
		Application();
		virtual ~Application();							//֧�ֶ�̬

		void Run();

		void OnEvent(Event& e);							//�¼��ַ�

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }			//�����������ָ��Window��ָ��
		inline static Application& Get() { return *s_Instance; }	//! ! !���ص���s_Instance���ָ��Application��ָ��
																	//��Ϊʲô���������ô��ݣ�����Ϊapplication��һ�������������ʹ��&�����һ�����ƣ�������ڵ���ģʽ��ֻ��һ�������Ҫ��
	private:
		bool OnWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;				//ָ��Window��ָ��
		std::unique_ptr<Shader> m_Shader;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;

		unsigned int m_VertexBuffer, m_VertexArray, m_IndexBuffer;
	private:
		static Application* s_Instance;					//! ! !Ψһʵ���ľ�̬��Ա��static���ͣ���Ҫ��ʼ�����壩
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}