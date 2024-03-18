#pragma once

#include "Nut/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Nut
{

	class WindowsWindow : public Window							//�����������û��NUT_API��ʶ
	{
	public:
		WindowsWindow(const WindowProps& props);				//�����˹��캯��
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		//Window attributes���麯����д��������
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);			//��ΪWindowsWindows������һЩ�ӿ�
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;									//������һ��GLFW�Ĵ��ڶ���

		struct WindowData
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}