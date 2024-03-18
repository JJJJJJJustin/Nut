#pragma once

#ifdef NUT_PLATFORM_WINDOWS

extern Nut::Application* Nut::CreateApplication();

int main(int argc, char** argv)				//将其设置为windows平台上的win main函数，可能会要求其返回一些状态信息
{

	auto app = Nut::CreateApplication();
	app->Run();
	delete app;
}
#endif