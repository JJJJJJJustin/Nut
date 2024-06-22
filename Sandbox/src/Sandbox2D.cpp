#include "Sandbox2D.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_Texture = Nut::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Nut::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Nut::RendererCommand::Clear();

	Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f});
	Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, { 0.2f, 0.8f, 0.3f, 1.0f });
	Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);
	Nut::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Test");
	ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
