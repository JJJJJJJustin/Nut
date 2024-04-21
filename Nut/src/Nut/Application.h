#pragma once

#include "Core.h"

#include "Nut/Events/ApplicationEvent.h"

#include "Window.h"
#include "LayerStack.h"
#include "Nut/ImGui/ImGuiLayer.h"

#include "Nut/Renderer/VertexArray.h"
#include "Nut/Renderer/Shader.h"
#include "Nut/Renderer/Buffer.h"
#include "Nut/Renderer/OrthoGraphicCamera.h"

namespace Nut {

	class NUT_API Application							//��ĵ���
	{
	public:
		Application();
		virtual ~Application();							//֧�ֶ�̬

		void Run();

		void OnEvent(Event& e);							//�¼��ַ�

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }			//�����������ָ��Window��ָ��
		inline static Application& Get() { return *s_Instance; }	//! ! !���ص���s_Instance���ָ��Application��ָ��
																	//��Ϊʲô���������ô��ݣ�����Ϊapplication��һ�������������ʹ��&�����һ�����ƣ�������ڵ���ģʽ��ֻ��һ�������Ҫ��
	private:
		bool OnWindowClose(WindowCloseEvent& event);

		bool m_Running = true;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		std::unique_ptr<Window> m_Window;				//ָ��Window��ָ��
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_SquareShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthoGraphicCamera m_Camera;
	private:
		static Application* s_Instance;					//! ! !Ψһʵ���ľ�̬��Ա��static���ͣ���Ҫ��ʼ�����壩
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}