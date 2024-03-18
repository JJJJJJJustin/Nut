#pragma once

#include "Nut/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Nut
{

	class WindowsWindow : public Window							//！！！这个类没有NUT_API标识
	{
	public:
		WindowsWindow(const WindowProps& props);				//声明了构造函数
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		//Window attributes（虚函数重写的声明）
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);			//又为WindowsWindows设置了一些接口
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;									//声明了一个GLFW的窗口对象

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