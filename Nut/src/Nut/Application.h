#pragma once

#include "Core.h"
#include "Window.h"

namespace Nut {

	class NUT_API Application							//类的导出
	{
	public:
		Application();
		virtual ~Application();							//支持多态

		void Run();
	private:
		std::unique_ptr<Window> m_Window;				//指向Windows的指针
		bool m_Running = true;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}