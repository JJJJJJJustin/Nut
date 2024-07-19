#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	NUT_PROFILE_FUNCTION();

	m_Texture = Nut::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	NUT_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Nut::Timestep ts)
{
	NUT_PROFILE_FUNCTION();												// 一个作用域只能声明一个 Timer 变量
	// Update
	m_CameraController.OnUpdate(ts); 

	// Render
	{
		NUT_PROFILE_SCOPE("RenderCommand Prep");
		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Nut::RendererCommand::Clear();
	}
	{
		NUT_PROFILE_SCOPE("Renderer2D Draw");

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f }, { 1.0f, 1.0f },{ 0.8f, 0.2f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, { 0.2f, 0.8f, 0.3f, 1.0f });
		//Nut::Renderer2D::DrawRotatedQuad({-1.0f, 1.0f}, {1.0f, 1.0f}, 45.0f, { 0.3f, 0.2f, 0.8f, 1.0f });
		//Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f, glm::vec4(0.9f, 1.0f, 1.0f, 1.0f));
		Nut::Renderer2D::EndScene(); 
	}
}

void Sandbox2D::OnImGuiRender()
{
	NUT_PROFILE_FUNCTION();
	
	ImGui::Begin("Test");
	ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
