#include "nutpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "GLFW/glfw3.h"

#include "Nut/Application.h"

namespace Nut
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();							//创建用于输入输出的对象
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	//后端支持自定义鼠标指针，并告诉 Dear ImGui 开启鼠标指针功能。
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	//后端支持设置鼠标位置，并告诉 Dear ImGui 开启设置鼠标位置的功能。

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array. 
		// AND SHOULD USE NUT KEY CODES EVENTUALLY
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");		//初始化 ImGui 在 OpenGL3 或更高版本环境下的渲染器,并使用 GLSL 4.10 版本进行着色器编译和处理。
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();		//！！！Get是静态函数，GetWindow是成员函数，使用方法不同
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetWidth() / 2);

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();				//每一帧开始时准备 OpenGL 渲染环境以供 Dear ImGui 绘制 UI 元素
		ImGui::NewFrame();							//清除之前的 UI 数据，准备接受新的 UI 绘制指令（准备新帧），并更新输入状态

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();							//结束当前帧的 UI 绘制过程，并准备将绘制的数据保存到暂存区
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	//将缓冲区中的绘制数据提交给渲染器进行操作 
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}
}