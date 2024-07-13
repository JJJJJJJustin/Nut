#include "Nut.h"
#include <Nut/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"


class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{
		//ȡ���� sandbox�� PushOverlay(new Nut::ImGuiLayer()); ��������Ϊ Hazel ����ʱ�Զ���ӵ�ͼ��:DemoWindow���� application.cpp �й̶����� OnImGuiRender ������
		PushLayer(new ExampleLayer());
		//PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}