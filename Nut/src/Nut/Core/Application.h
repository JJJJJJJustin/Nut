#pragma once

#include "Nut/Core/Base.h"

#include "Nut/Events/Event.h"
#include "Nut/Events/ApplicationEvent.h"

#include "Nut/Core/Window.h"
#include "Nut/Core/LayerStack.h"
#include "Nut/ImGui/ImGuiLayer.h"

#include "Nut/Core/Timestep.h"

int main(int argc, char** argv);

namespace Nut {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);							//�¼��ַ�

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }			//�����������ָ��Window��ָ��
		inline static Application& Get() { return *s_Instance; }	//! ! !���ص���s_Instance���ָ��Application��ָ��
																	//��Ϊʲô���������ô��ݣ�����Ϊapplication��һ�������������ʹ��&�����һ�����ƣ�������ڵ���ģʽ��ֻ��һ�������Ҫ��
	private:
		void Run();													// Run ��������Ϊ˽�У� Run �����ж��� RunLoop��

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
	private:
		bool m_Running = true;
		bool m_Minimized = false;
		std::unique_ptr<Window> m_Window;				//ָ��Window��ָ��
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;					//! ! !Ψһʵ���ľ�̬��Ա��static���ͣ���Ҫ��ʼ�����壩
		friend int ::main(int argc, char** argv);		// ͨ���� main ����Ϊ��Ԫ��������������ⲿͨ�� main ������˽�е� Run ����
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}