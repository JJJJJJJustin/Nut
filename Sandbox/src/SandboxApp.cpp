#include "Nut.h"

#include "imgui/imgui.h"

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer()
		:Layer("Example layer")
	{}

	void OnUpdate() override
	{
		//NUT_INFO("ExampleLayer::Updata");
	}

	void OnImGuiRender()
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello world");
		ImGui::End();
	}

	void OnEvent(Nut::Event& event) override
	{
		//NUT_TRACE("{0}", event);
		if (event.GetEventType() == Nut::EventType::KeyPressed)
		{
			Nut::KeyPressedEvent& e = (Nut::KeyPressedEvent&)event;			//将参数中未知的事件event转换成相应类型，并传递给e
			if (e.GetKeyCode() == NUT_KEY_TAB)
				NUT_TRACE("{0} is pressed", "Tab");
			NUT_TRACE("{0} is pressed",(char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//取消 PushOverlay(new Nut::ImGuiLayer()); ，将其作为 Hazel 运行时 固定自动添加的图层
	}

	~Sandbox()
	{

	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}