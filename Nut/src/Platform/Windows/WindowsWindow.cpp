#include "nutpch.h"
#include "Platform\Windows\WindowsWindow.h"

#include "Nut\Events\ApplicationEvent.h"
#include "Nut\Events\KeyEvent.h"
#include "Nut\Events\MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"


namespace Nut
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error_code, const char* description)
	{
		NUT_CORE_ERROR("GLFW Error ({0}):{1}", error_code, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		NUT_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		NUT_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		NUT_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		NUT_CORE_INFO("Creating window: {0} ({1} , {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			NUT_PROFILE_SCOPE("glfwInitWindow");

			NUT_CORE_INFO("Initializing GLFW window..");
			int success = glfwInit();
			NUT_CORE_ASSERT(success, "Could not intialize GLFW!");

			//If Initalized successfully but get the wrong on run time
			glfwSetErrorCallback(GLFWErrorCallback);
		}
		{
			NUT_PROFILE_SCOPE("glfwCreateWindow");

			//初始化Windows对象并创建窗口上下文
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr); 

			s_GLFWWindowCount++;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		#pragma region ------------------Set GLFW callbacks FN(function) in function "init()"----------------------
		glfwSetWindowSizeCallback(m_Window,
			[](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			}
		);																	//lambda表达式作为第二个参数

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);									//data.EventCallback中存入了OnEvent函数（在application中），event作为其参数使用
			}
		);

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS: {KeyPressedEvent event(key, 0);	data.EventCallback(event);	break; }
				case GLFW_RELEASE: {KeyReleasedEvent event(key);	data.EventCallback(event);	break; }
				case GLFW_REPEAT: {KeyPressedEvent event(key, 1);	data.EventCallback(event);	break; }
				}
			}
		);

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			}
		);

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS: { MouseButtonPressedEvent event(button);	data.EventCallback(event);	break; }
				case GLFW_RELEASE: {MouseButtonReleasedEvent event(button);	data.EventCallback(event);	break; }
				}
			}
		);

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			}
		);

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xpos, (float)ypos);
				data.EventCallback(event);
			}
		);
		#pragma endregion
	}

	void WindowsWindow::Shutdown()
	{
		NUT_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0) {
			NUT_CORE_INFO("Terminating GLFW..");
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		NUT_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)							//是否启用垂直同步
	{
		NUT_PROFILE_FUNCTION();

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