#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//将其设置为windows平台上的win main函数，可能会要求其返回一些状态信息
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