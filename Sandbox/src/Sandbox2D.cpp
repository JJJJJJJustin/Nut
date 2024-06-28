#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
// -----------------------Timer---------------------------------------
struct ProfileResult
{
	const char* Name;
	float Time;
};
std::vector<ProfileResult> m_ProfileResults;

class Timer
{
public:
	Timer(const char* name)
		:m_Name(name),m_Stopped(false)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	~Timer() 
	{
		if (!m_Stopped) 
		{
			Stop();
		}
	}

	void Stop() 
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();				//TYPE : std::chrono::time_point<std::chrono::steady_clock>
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
		long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
		
		m_Stopped = true;
		float duration = (end - start) * 0.001f;
		MesAttrib({m_Name, duration});
	}

	void MesAttrib(const ProfileResult& profileResult)
	{
		m_ProfileResults.push_back(profileResult);
	}

private:
	const char* m_Name;
	bool m_Stopped;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
};

#define PROFILE_SCOPE(name) Timer timer_##__LINE__(name)

// --------------------------------------------------------------------
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
	PROFILE_SCOPE("Sandbox2D::OnUpdate");					// 一个作用域只能声明一个 Timer 变量
	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts); 
	}
	// Render
	{
		PROFILE_SCOPE("RenderCommand Prep");
		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Nut::RendererCommand::Clear();
	}
	{
		PROFILE_SCOPE("Renderer2D Draw");

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, { 0.2f, 0.8f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawQuad({ 0.0f,  0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);
		Nut::Renderer2D::EndScene(); 
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Test");
	ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_SquareColor));

	for(auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
