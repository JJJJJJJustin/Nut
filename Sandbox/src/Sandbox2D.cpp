#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>


static const char* s_GameMap = {
	"AAAAAAAAAAAAAAAAAMMA"
	"AAAAAAAAAAAAAAAALLLL"
	"AAAAAAAAAAAAAAAADDDD"
	"SAAAAAAAAAAAAAAAAAAA"
	"LWWWWLAAAAAAAAAAABAA"
	"LWWWWLLLLAAAAAAAAAAA"
	"DDDDDDDDDAAAALLLLGGL"
	"AAAAAAAAAAAAADDDDDDD"
	"AAAAAAAAAAAAAAAAAAAA"
	"AAALLLLAALLLLLLLLLLL"
	"GGGDDDDGGDDDDDDDDDDD"
	"DDDDDDDDDDDDDDDDDDDD" };
static const  uint32_t s_MapWidth = 20;
static const  uint32_t s_MapHeight = strlen(s_GameMap) / s_MapWidth;


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	NUT_PROFILE_FUNCTION();

	m_Framebuffer = Nut::FrameBuffer::Create({ 1280, 720 });

	m_Texture = Nut::Texture2D::Create("assets/textures/Checkerboard.png");
	m_Emoji = Nut::Texture2D::Create("assets/textures/emoji.png");
	
	// Test 1
	m_SpriteSheet = Nut::Texture2D::Create("assets/game/textures/tilemap_packed.png");
	m_TilesMap['D'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 4, 8 } /* ,{1,1} */);
	m_TilesMap['G'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 0, 8 });
	m_TilesMap['L'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 0, 6 });
	m_TilesMap['W'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 13, 5 });
	m_TilesMap['B'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 9, 8 });
	m_TilesMap['K'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 7, 7 });
	m_TilesMap['F'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 11, 3 });
	m_TilesMap['M'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, { 8, 2 });
	m_TilesMap['S'] = Nut::SubTexture2D::Create(m_SpriteSheet, { 18, 18 }, {  5, 1 });
	// Test2
	m_SpriteSheetRole = Nut::Texture2D::Create("assets/game/textures/tilemap(2).png");
	m_Role = Nut::SubTexture2D::Create(m_SpriteSheetRole, { 16, 16 }, { 0, 3 }, { 1, 1 });

	#pragma region Particle Init here
	m_Particle.ColorBegin = { 138 / 255.0f, 43 / 255.0f, 226 / 255.0f, 1.0f };
	m_Particle.ColorEnd =   { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.3f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
	#pragma endregion

	m_CameraController.SetZoomLevel(5.0f);
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
		m_Framebuffer->Bind();											// 在颜色被设置之前就声明帧缓冲
		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Nut::RendererCommand::Clear();
	}
	{
		NUT_PROFILE_SCOPE("Renderer2D Draw");

#if 1
		static float temp = 0.0f;
		temp += ts * 100.0f;

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Nut::Renderer2D::DrawQuad({ 1.0f,  1.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Nut::Renderer2D::DrawQuad({ 1.0f, -1.0f }, { 0.5f, 1.0f }, m_QuadColor);
		Nut::Renderer2D::DrawRotatedQuad({ -2.0f, -0.0f }, { 1.0f, 1.0f }, glm::radians(temp), m_Emoji);
		Nut::Renderer2D::DrawQuad({ -0.0f,  -0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		Nut::Renderer2D::EndScene();

		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { 0.0f ,(x + 5.0f) / 10.0f, (y + 5.0f) / 10.0f, 0.7f };
				Nut::Renderer2D::DrawQuad({ x,y }, { 0.45f, 0.45f }, color);
			}
		}
		Nut::Renderer2D::EndScene();
#endif

#if 0
		if (Nut::Input::IsMouseButtonPressed(NUT_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Nut::Input::GetMousePos();							// 获取窗口中鼠标当前的位置
			auto width = Nut::Application::Get().GetWindow().GetWidth();		// 获取窗口宽高
			auto height = Nut::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();						// 获取相机边界（相机的视景体->平截头体），即相机能观察到的空间区域
			auto pos = m_CameraController.GetCamera().GetPosition();			// 获取相机在空间中的位置
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 1; i++)
				m_ParticleSystem.Emit(m_Particle);
		}
		m_ParticleSystem.OnUpdate(ts);											// 更新信息
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());				// 渲染粒子



		Nut::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (uint32_t x = 0; x < s_MapWidth; x++) {
			for (uint32_t y = 0; y < s_MapHeight; y++)
			{
				char keyChar = s_GameMap[x + y * s_MapWidth];

				Nut::Ref<Nut::SubTexture2D> subTexture;
				if (m_TilesMap.find(keyChar) != m_TilesMap.end()) {
					subTexture = m_TilesMap[keyChar];
					Nut::Renderer2D::DrawQuad({ x, s_MapHeight - y , 0.0f }, { 1.0f, 1.0f }, subTexture);
				}
				else
					Nut::Renderer2D::DrawQuad({ x, s_MapHeight - y, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
			}
		}
		Nut::Renderer2D::DrawQuad({ 18.0f, 7.0f, 0.5f }, { 1.0f, 1.0f }, m_Role);

		Nut::Renderer2D::EndScene();
#endif
		m_Framebuffer->Unbind();
	}
}

void Sandbox2D::OnImGuiRender()
{
	NUT_PROFILE_FUNCTION();
	
	static bool dockingEnable = true;
	if (dockingEnable) {
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				/*if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
				if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
				if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }*/
				if (ImGui::MenuItem("Exit")) { Nut::Application::Get().WindowClose(); }

				/*ImGui::Separator();
				if (ImGui::MenuItem("Close", NULL, false))
					dockspaceOpen = false;*/

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// ----------- Should be writen in Dockspace( Between dockspace's ImGui::Begin() <-> ImGui::End() ) ----
		ImGui::Begin("Test");
		auto stats = Nut::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());

		ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_QuadColor));

		ImTextureID textureID = (void*)m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(textureID, ImVec2{ 1280.0f, 720.0f });

		ImGui::End();
		// -------------------------------------------------------------------------------------------------------

		ImGui::End();
	}
	else {
		ImGui::Begin("Test");
		auto stats = Nut::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());

		ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_QuadColor));

		ImTextureID textureID = (void*)m_Emoji->GetRendererID();
		ImGui::Image(textureID, ImVec2{ 256.0f, 256.0f });

		ImGui::End();
	}
}

void Sandbox2D::OnEvent(Nut::Event& event)
{
	m_CameraController.OnEvent(event);
}
