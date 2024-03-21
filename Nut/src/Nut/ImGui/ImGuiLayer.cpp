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

		ImGuiIO& io = ImGui::GetIO();							//����������������Ķ���
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	//���֧���Զ������ָ�룬������ Dear ImGui �������ָ�빦�ܡ�
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	//���֧���������λ�ã������� Dear ImGui �����������λ�õĹ��ܡ�

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

		ImGui_ImplOpenGL3_Init("#version 410");		//��ʼ�� ImGui �� OpenGL3 ����߰汾�����µ���Ⱦ��,��ʹ�� GLSL 4.10 �汾������ɫ������ʹ���
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();		//������Get�Ǿ�̬������GetWindow�ǳ�Ա������ʹ�÷�����ͬ
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetWidth() / 2);

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();				//ÿһ֡��ʼʱ׼�� OpenGL ��Ⱦ�����Թ� Dear ImGui ���� UI Ԫ��
		ImGui::NewFrame();							//���֮ǰ�� UI ���ݣ�׼�������µ� UI ����ָ�׼����֡��������������״̬

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();							//������ǰ֡�� UI ���ƹ��̣���׼�������Ƶ����ݱ��浽�ݴ���
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	//���������еĻ��������ύ����Ⱦ�����в��� 
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}
}