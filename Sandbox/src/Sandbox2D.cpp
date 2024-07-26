#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	NUT_PROFILE_FUNCTION();

	m_Texture = Nut::Texture2D::Create("assets/textures/Checkerboard.png");
	m_Emoji = Nut::Texture2D::Create("assets/textures/emoji.png");

	#pragma region Particle Init here
	m_Particle.ColorBegin = { 138 / 255.0f, 43 / 255.0f, 226 / 255.0f, 1.0f };
	m_Particle.ColorEnd =   { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.3f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
	#pragma endregion
}

void Sandbox2D::OnDetach()
{
	NUT_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Nut::Timestep ts)
{
	NUT_PROFILE_FUNCTION();												// һ��������ֻ������һ�� Timer ����
	// Update
	m_CameraController.OnUpdate(ts); 

	// Render
	Nut::Renderer2D::ClearStats();										// ÿ�θ���ǰ��Ҫ��Statsͳ����������
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
		Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, m_QuadColor );
		Nut::Renderer2D::DrawRotatedQuad({-2.0f, -0.0f}, {1.0f, 1.0f}, glm::radians(temp), m_Emoji);
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

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		if (Nut::Input::IsMouseButtonPressed(NUT_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Nut::Input::GetMousePos();							// ��ȡ��������굱ǰ��λ��
			auto width  = Nut::Application::Get().GetWindow().GetWidth();		// ��ȡ���ڿ��
			auto height = Nut::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();						// ��ȡ����߽磨������Ӿ���->ƽ��ͷ�壩��������ܹ۲쵽�Ŀռ�����
			auto pos = m_CameraController.GetCamera().GetPosition();			// ��ȡ����ڿռ��е�λ��
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 1; i++)
				m_ParticleSystem.Emit(m_Particle);
		}
		m_ParticleSystem.OnUpdate(ts);											// ������Ϣ
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());				// ��Ⱦ����
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

	ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_QuadColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
