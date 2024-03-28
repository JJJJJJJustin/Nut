#include "Nut.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer()
		:Layer("Example layer")
	{
		auto cam = camera(5.0f, { 0.5f,0.5f });
	}

	void OnUpdate() override
	{
		//NUT_INFO("ExampleLayer::Updata");
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