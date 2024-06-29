#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//��������Ϊwindowsƽ̨�ϵ�win main���������ܻ�Ҫ���䷵��һЩ״̬��Ϣ
{
	Nut::Log::Init();
	NUT_CORE_WARN("Initialized Log!");
	NUT_INFO("Goodbye World!");

	NUT_PROFILE_BEGIN_SESSION("Startup", "NutProfile-Startup.json");
	auto app = Nut::CreateApplication();
	NUT_PROFILE_END_SESSION();

	NUT_PROFILE_BEGIN_SESSION("Runtime", "NutProfile-Runtime.json");
	app->Run();
	NUT_PROFILE_END_SESSION();

	NUT_PROFILE_BEGIN_SESSION("Shutdown", "NutProfile-Shutdown.json");
	delete app;
	NUT_PROFILE_END_SESSION();
}
#endif