#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//��������Ϊwindowsƽ̨�ϵ�win main���������ܻ�Ҫ���䷵��һЩ״̬��Ϣ
{

	auto app = Nut::CreateApplication();
	app->Run();
	delete app;
}
#endif