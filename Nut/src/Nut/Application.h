#pragma once

#include "Core.h"

namespace Nut {

	class NUT_API Application							//类的导出
	{
	public:
		Application();
		virtual ~Application();							//支持多态

		void Run();
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}