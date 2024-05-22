#pragma once

#include "Nut/Core/Window.h"
#include "Nut/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Nut
{

	class WindowsWindow : public Window							//！！！这个类没有NUT_API标识（作为静态库使用）
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

		inline void* GetNativeWindow() const override { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);			//又为WindowsWindows设置了一些接口
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;									//声明了一个GLFW的窗口对象
		GraphicsContext* m_Context;								//声明一个上下文对象（GraphicsContext类 没有构造函数，在.cpp中为其new一个指针对象）

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