#pragma once

#include "Core.h"

namespace Nut {

	class NUT_API Application							//��ĵ���
	{
	public:
		Application();
		virtual ~Application();							//֧�ֶ�̬

		void Run();
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}