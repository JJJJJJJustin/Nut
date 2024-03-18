#pragma once

#include "Core.h"
#include "Window.h"

#include "Nut/Events/ApplicationEvent.h"

namespace Nut {

	class NUT_API Application							//��ĵ���
	{
	public:
		Application();
		virtual ~Application();							//֧�ֶ�̬

		void OnEvent(Event& e);							//�¼��ַ�
		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;				//ָ��Windows��ָ��
		bool m_Running = true;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}