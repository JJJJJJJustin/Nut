// OSAlgorithm
#include "EditorLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include "Nut/Scene/ScriptableEntity.h"
#include "Nut/Scene/SceneSerializer.h"
#include "Nut/Utils/PlatformUtils.h"

#include <ImGuizmo.h>

#include "Nut/Math/Math.h"

#include "Algorithm.h"

namespace Nut {
	static float xTrans = 0.0f;

	EditorLayer* EditorLayer::s_Instance = nullptr;		// Initialize s_Instance as null, then give it a value(this pointer) in constructor

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
	{
		NUT_CORE_ASSERT(!s_Instance, "EditorLayer Instance already exists!(EditorLayer is a Singleton!)");
		s_Instance = this;
	}

	void EditorLayer::OnAttach()
	{
		NUT_PROFILE_FUNCTION();

		m_Framebuffer = FrameBuffer::Create({ 1280, 720, 1, {FrameBufferAttachmentFormat::RGBA8, FrameBufferAttachmentFormat::Depth} });

		m_ActiveScene = CreateRef<Scene>();

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

#if 0
		srand(static_cast<unsigned int>(time(0)));	// 设置随机种子
		int num_processes = rand() % 5 + 4;			// 生成4到8个进程
		std::vector<Process> processes;

		PriorityAlgorithm::generate_processes(processes, num_processes);

		std::vector<Process> completedProcesses = PriorityAlgorithm::priority_scheduling(processes);

		float deep = 1.0f;
		for (auto& process : completedProcesses)
		{
			Entity squareEntity = m_ActiveScene->CreateEntity(process.name);

			squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f });

			auto& tc = squareEntity.GetComponent<TransformComponent>();
			tc.Translation = glm::vec3{ process.arrival_time + (process.turnaround_time / 2), 0, deep };
			tc.Scale = glm::vec3{ process.turnaround_time, 1, 1 };

			deep -= 0.1f;
		}

		m_CameraEntity = m_ActiveScene->CreateEntity("Main-Camera");
		auto& firstController = m_CameraEntity.AddComponent<CameraComponent>();
		firstController.Primary = true;
		//m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<ScriptCameraController>();			//添加本机脚本
#endif
#if 0
		srand(static_cast<unsigned>(time(0)));	// 设置随机种子
		int num_processes = rand() % 5 + 4;		// 进程数在4到8之间
		int time_segment = 5;					// 时间片
		std::vector<Process> processes;

		RoundAlgorithm::generate_processes(processes, num_processes);
		RoundAlgorithm::round_robin(processes, time_segment, "E:/VS/Nut/OSAlgorithms/assets/data/data.txt");
		std::vector<std::pair<std::string, int>> completedProcesses = Tool::ReadFile("E:/VS/Nut/OSAlgorithms/assets/data/data.txt");

		int i = 0;
		float translateX = 0;

		for (auto& process : completedProcesses)
		{
			translateX += process.second;

			if (process.first == "P1")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P2")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P3")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P4")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P5")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P6")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P7")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P8")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.5f, 0.0f, 0.5f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P9")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.5f, 0.5f, 0.0f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}
			else if (process.first == "P10")
			{
				std::string name = process.first + '-' + std::to_string(i);
				Entity squareEntity = m_ActiveScene->CreateEntity(name);

				squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.3f, 0.1f, 0.5f, 1.0f });

				auto& tc = squareEntity.GetComponent<TransformComponent>();
				tc.Translation = glm::vec3{ translateX, 0, 0 };
				tc.Scale = glm::vec3{ process.second ,1, 1 };
			}

			i++;
		}

#endif
#if 0
		int n = rand() % 5 + 4; // 随机生成4~8个进程
		int m = 4; // 资源种类固定为4
		Banker banker(n, m);

		banker.printStatus();

		// 测试资源请求
		//banker.requestResources(0, { 1, 1, 1, 1 });
		//banker.releaseResources(0, { 0, 1, 0, 1 });
		banker.requestResources(1, { 2, 0, 2, 0 });
#endif
#if 0
		StorageManage::GenerateInstructions("E:/Instructions.txt");
		StorageManage::ExecuteInstructions("E:/Instructions2.txt");

		// 测试不同内存大小从 2K 到 32K
		for (int numFrames = 2; numFrames <= 32; numFrames *= 2) {
			std::cout << "\n内存大小-> " << numFrames * 1024 << " 字节 (物理块: " << numFrames << ")\n";

			std::cout << "FIFO: " << StorageManage::FIFO(StorageManage::GetPages(), numFrames) << "\t";
			std::cout << "LRU: " << StorageManage::LRU(StorageManage::GetPages(), numFrames) << "\t";
			std::cout << "OPT: " << StorageManage::OPT(StorageManage::GetPages(), numFrames) << "\t";
			std::cout << "Clock: " << StorageManage::Clock(StorageManage::GetPages(), numFrames) << "\t\n";
		}


		DrawResults("FIFO", StorageManage::FIFO);

		xTrans += 2.0f;
		DrawResults("LRU", StorageManage::LRU);

		xTrans += 2.0f;
		DrawResults("OPT", StorageManage::OPT);

		xTrans += 2.0f;
		DrawResults("Clock", StorageManage::Clock);

#endif
#if 1
		std::srand(std::time(0));

		std::vector<int> requests(10);
		int start = 100;						// 磁头初始位置

		// 填充数组，生成0到199之间的随机值
		for (int i = 0; i < 10; i++) {
			requests[i] = std::rand() % 200;	// 生成0到199之间的随机数
		}

		DiskScheduler scheduler(requests, start);

		// SCAN 算法
		m_ProcessedScanSeq = scheduler.Scan();
		std::cout << "SCAN Sequence: ";
		scheduler.Print(m_ProcessedScanSeq);

		// CSCAN 算法
		m_ProcessedCscanSeq = scheduler.Cscan();
		std::cout << "CSCAN Sequence: ";
		scheduler.Print(m_ProcessedCscanSeq);

		Entity entity = m_ActiveScene->CreateEntity("Disk Track");

		entity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Translation = glm::vec3{ 0.0f, 0.0f, 0.0f };
		tc.Scale = glm::vec3{ 10.0f, 1.0f, 1.0f };

		for (size_t i = 0; i < requests.size(); i++)
		{
			std::string name = "Location" + std::to_string(requests[i]);

			Entity squareEntity = m_ActiveScene->CreateEntity(name);

			squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 0.3f, 0.0f, 1.0f });

			auto& tc = squareEntity.GetComponent<TransformComponent>();
			tc.Translation = glm::vec3{ (requests[i] - 100.0) / 100.0 * 5.0, 0.0f, 0.3f };
			tc.Scale = glm::vec3{ 0.03f, 1.0f, 1.0f };
		}

		m_ArrowEntity = m_ActiveScene->CreateEntity("Arrow"); 
		m_ArrowEntity.AddComponent<SpriteComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
		m_ArrowEntity.GetComponent<TransformComponent>().Scale = glm::vec3{ 0.0f };
#endif

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		NUT_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		NUT_PROFILE_FUNCTION();												// 一个作用域只能声明一个 Timer 变量

		// Logic Update
		if ((m_ViewportSize.x != m_Framebuffer->GetSpecification().Width || m_ViewportSize.y != m_Framebuffer->GetSpecification().Height)
			&& m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Camera Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);
		m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ClearStats();										// 每次更新前都要将Stats统计数据清零
		m_Framebuffer->Bind();											// 在颜色被设置之前就声明帧缓冲

		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RendererCommand::Clear();

		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
		//m_ActiveScene->OnScript(ts);								// 更新本机脚本

		m_Framebuffer->Unbind();

	}

	void EditorLayer::OnImGuiRender()
	{
		NUT_PROFILE_FUNCTION();

#pragma region PrepareDockspace
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
		ImGuiStyle& style = ImGui::GetStyle();

		float minWinSizeX = style.WindowMinSize.x;	// temporary variable stroes minimum value
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;		// After dockspace was created, turn the window minimum size back

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
				if (ImGui::MenuItem("New", "Ctrl + N"))
					NewScene();
				if (ImGui::MenuItem("Save As...", "Ctrl + S"))
					SaveSceneAs();
				if (ImGui::MenuItem("Open...", "Ctrl + O"))
					OpenScene();
				if (ImGui::MenuItem("Exit"))
					Application::Get().WindowClose();

				/*ImGui::Separator();
				if (ImGui::MenuItem("Close", NULL, false))
					dockspaceOpen = false;*/

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
#pragma endregion
		// ----------- Should be writen in Dockspace( Between dockspace's ImGui::Begin() <-> ImGui::End() ) ----
		// ----------- Hierarchy Panel -------------------------------------------
		m_SceneHierarchyPanel.OnImGuiRender();
		// ----------- Test Panel---------------------------------------------
		ImGui::Begin("Stats");
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());

		ImGui::Separator();
		if (ImGui::Button("Run Scan")) 
		{
			NUT_CORE_INFO("Scan sequence is running now");
			DrawArrow(m_ProcessedScanSeq);
		}
		ImGui::SameLine();
		if (ImGui::Button("Run Cscan"))
		{
			NUT_CORE_INFO("Cscan sequence is running now")
			DrawArrow(m_ProcessedCscanSeq);
		}

		ImGui::End();
		// ----------- Viewport Image --------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		// Viewport
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { panelSize.x, panelSize.y };

		ImTextureID textureID = (void*)m_Framebuffer->GetColorAttachmentRendererID();				// Defined in OpenGLFramebuffer.h
		ImGui::Image(textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			// Set operating enviroment
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());

			// Get camera projection matrix & camera view matrix & transform matrix
			Entity cameraEntity = m_ActiveScene->GetPrimaryCamera();
#if 0		// Draw gizmos for the entities we create in Actual-Game(Which will use Runtime camera to check things)
			// Camera Projection
			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const auto& cameraProjection = camera.GetProjection();
			// Camera View
			auto& cameraTransform = cameraEntity.GetComponent<TransformComponent>().GetTransform();
			glm::mat4 cameraView = glm::inverse(cameraTransform);
#endif
#if 1		// Draw gizmos for the entities we create in Nut-Editor(Which will use Editor camera to check things)
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			const glm::mat4& cameraView = m_EditorCamera.GetViewMatrix();
#endif
			// Transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			float snapValue = 0.5f;
			bool snap = Input::IsKeyPressed(NUT_KEY_LEFT_CONTROL);
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snap = 10.0f;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			// Manipulate
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				ImGuizmo::OPERATION(m_GizmoType), ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);
				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
		// -------------------------------------------------------------------------------------------------------

		ImGui::End();

	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
		m_EditorCamera.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(NUT_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}


	// ------------------------------------------------------------------------------------------------------------
	// --------------------------------------- Some definations ---------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		// You can triggering this event only once,
		// because if you try to tirgger this event again, 
		// the event.GetRepeatCount will biger than 0.
		// And function will return false
		if (event.GetRepeatCount() > 0)
			return false;

		bool ctrl = Input::IsKeyPressed(NUT_KEY_LEFT_CONTROL) || Input::IsKeyPressed(NUT_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(NUT_KEY_LEFT_SHIFT) || Input::IsKeyPressed(NUT_KEY_RIGHT_SHIFT);
		switch (event.GetKeyCode())
		{
			// Dialog
			case NUT_KEY_N: {
				if (ctrl)
					NewScene();

				break;
			}
			case NUT_KEY_O: {
				if (ctrl)
					OpenScene();

				break;
			}
			case NUT_KEY_S: {
				if (ctrl && shift)
					SaveSceneAs();

				break;
			}

			// Gizmo
			case NUT_KEY_Q:
			{
				m_GizmoType = -1;
				break;
			}
			case NUT_KEY_W:
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case NUT_KEY_E:
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case NUT_KEY_R:
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Nut Scene(*.yaml)\0 * .yaml\0All Files (*.*)\0*.*\0\0");	// ????why filedialogs need to return string
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);// We use it cuz we must flash framebuffer after we open file
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);										// We use it cuz we need to flash the data / result which is rendered in hierarchy panel

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Nut Scene()(*.yaml)\0 * .yaml\0");
		if (!filepath.empty())
		{
			SceneSerializer deserializer(m_ActiveScene);
			deserializer.Serialize(filepath);
		}
	}

	void EditorLayer::DrawResults(const std::string& algorithmName, std::function<double(std::vector<int>, int)> func)
	{
		for (int numFrames = 2; numFrames <= 32; numFrames *= 2)
		{
			float scale = func(StorageManage::GetPages(), numFrames);

			std::stringstream entityName;
			entityName << algorithmName << " with" << numFrames << "Frames";

			Entity squareEntity = m_ActiveScene->CreateEntity(entityName.str());
			squareEntity.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f });
			auto& tc = squareEntity.GetComponent<TransformComponent>();
			tc.Translation = { xTrans, scale / 2, 0.0f };				// scale / 2 ：保证实体位于同一水平线
			tc.Scale = { 0.5f, scale, 1.0f };

			xTrans += 0.6f;
		}
	}

	void EditorLayer::DrawArrow(std::vector<int>& seq)
	{
		NUT_CORE_ASSERT(!seq.empty(), "sequence is empty")

		static size_t index = 0;

		if (index < seq.size()) {
			auto& tc = m_ArrowEntity.GetComponent<TransformComponent>();
			tc.Translation = glm::vec3{ (seq[index] - 100.0f) / 100.0f * 5.0f, 0.0f, 0.0f };
			tc.Scale = glm::vec3{ 0.1f, 1.0f, 1.0f };

			index += 1;
		}
		else
		{
			NUT_CORE_INFO("Scan sequence run completed");
		}
	}


}