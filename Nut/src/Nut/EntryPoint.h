#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//将其设置为windows平台上的win main函数，可能会要求其返回一些状态信息
{
	Nut::Log::Init();
	NUT_CORE_WARN("Initialized Log!");
	int a = 5;
	NUT_INFO("Hello ! Var={0}", a);

	auto app = Nut::CreateApplication();
	app->Run();
	delete app;
}
#endif