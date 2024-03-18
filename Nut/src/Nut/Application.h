#pragma once

#include "Core.h"
#include "Window.h"

namespace Nut {

	class NUT_API Application							//��ĵ���
	{
	public:
		Application();
		virtual ~Application();							//֧�ֶ�̬

		void Run();
	private:
		std::unique_ptr<Window> m_Window;				//ָ��Windows��ָ��
		bool m_Running = true;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}