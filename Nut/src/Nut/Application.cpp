#include "nutpch.h"
#include "Application.h"

#include "Events/Event.h"
#include "Nut/Events/ApplicationEvent.h"
#include "Nut/Log.h"

namespace Nut {

	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent WRE(1280, 720);
		if (WRE.IsInCategory(EventCategoryApplication))
		{
			NUT_TRACE(WRE);
		}
		if (WRE.IsInCategory(EventCategoryMouseButton))
		{
			NUT_TRACE(WRE);
		}
		while (true);
	}
}