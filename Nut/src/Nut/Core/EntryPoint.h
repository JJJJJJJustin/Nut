#pragma once
#include "Nut/Core/Base.h"

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)				//��������Ϊwindowsƽ̨�ϵ�win main���������ܻ�Ҫ���䷵��һЩ״̬��Ϣ(�� argc, argv �л�ȡ)
{
	Nut::Log::Init();

	NUT_CORE_WARN("Initialized Log!");
	NUT_INFO("Goodbye World!");
	NUT_ERROR("Goodbye World!");
	
	NUT_CORE_WARN("Command line args:");
	for (int i = 0; i < argc; i++) {
		NUT_CORE_TRACE("Argument {0}: {1}", i, argv[i])
	}

	NUT_PROFILE_BEGIN_SESSION("Startup", "NutProfile-Startup.json");
	auto app = Nut::CreateApplication({ argc, argv });
	NUT_PROFILE_END_SESSION();

	NUT_PROFILE_BEGIN_SESSION("Runtime", "NutProfile-Runtime.json");
	app->Run();
	NUT_PROFILE_END_SESSION();

	NUT_PROFILE_BEGIN_SESSION("Shutdown", "NutProfile-Shutdown.json");
	delete app;
	NUT_PROFILE_END_SESSION();
}


#endif