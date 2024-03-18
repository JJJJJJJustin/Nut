#include "nutpch.h"
#include "WindowsWindow.h"

namespace Nut
{
	static bool s_GLFWInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		NUT_CORE_TRACE("Creating window: {0} ({1} , {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)									//这样的语句使GLFW只会在整个周期中初始化一次（后续s_GLFWInitialized会改成true）
		{
			//TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			NUT_CORE_ASSERT(success, "Could not intialize GLFW!");

			s_GLFWInitialized = true;
		}
		//初始化Windows对象并创建窗口上下文
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)							//是否启用垂直同步
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}