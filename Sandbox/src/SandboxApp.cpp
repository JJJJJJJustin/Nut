#include "Nut.h"

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer()
		:Layer("Example layer")
	{}

	void OnUpdate() override
	{
		NUT_INFO("ExampleLayer::Updata");
	}

	void OnEvent(Nut::Event& event) override
	{
		NUT_TRACE("{0}", event);
	}
};

class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Nut::ImGuiLayer());
	}

	~Sandbox()
	{

	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}