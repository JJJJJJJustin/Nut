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
	m_Emoji = Nut::Texture2D::Create("assets/textures/emoji.png");
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
	Nut::Renderer2D::ClearStats();										// 每次更新前都要将Stats统计数据清零
	{
		NUT_PROFILE_SCOPE("RenderCommand Prep");
		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Nut::RendererCommand::Clear();
	}
	{
		NUT_PROFILE_SCOPE("Renderer2D Draw");

		static float temp = 0.0f;
		temp += ts * 100.0f;

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Nut::Renderer2D::DrawQuad({ 1.0f,  1.0f }, { 1.0f, 1.0f },{ 0.8f, 0.2f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, { 0.2f, 0.8f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawRotatedQuad({-2.0f, -0.0f}, {1.0f, 1.0f}, temp, m_Emoji);
		Nut::Renderer2D::DrawQuad({ -0.0f,  -0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		Nut::Renderer2D::EndScene(); 

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for(float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f) 
			{
				glm::vec4 color = { 0.0f ,(x + 5.0f) / 10.0f, (y + 5.0f) / 10.0f, 0.7f };
				Nut::Renderer2D::DrawQuad({ x,y }, { 0.45f, 0.45f }, color);
			}
		}
		Nut::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	NUT_PROFILE_FUNCTION();
	
	ImGui::Begin("Test");
	auto stats = Nut::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetVertexCount());
	ImGui::Text("Indices: %d", stats.GetIndexCount());

	ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
