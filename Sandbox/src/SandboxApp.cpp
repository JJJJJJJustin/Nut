#include "Nut.h"

class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}