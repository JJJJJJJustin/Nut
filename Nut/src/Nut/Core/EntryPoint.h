#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//��������Ϊwindowsƽ̨�ϵ�win main���������ܻ�Ҫ���䷵��һЩ״̬��Ϣ
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