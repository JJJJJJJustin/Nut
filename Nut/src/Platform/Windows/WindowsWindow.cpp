#include "nutpch.h"
#include "WindowsWindow.h"

#include "Nut\Events\ApplicationEvent.h"
#include "Nut\Events\KeyEvent.h"
#include "Nut\Events\MouseEvent.h"

namespace Nut
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error_code, const char* description)
	{
		NUT_CORE_ERROR("GLFW Error ({0}):{1}", error_code, description);
	}

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

		NUT_CORE_INFO("Creating window: {0} ({1} , {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)									//���������ʹGLFWֻ�������������г�ʼ��һ�Σ�����s_GLFWInitialized��ĳ�true��
		{
			//TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			NUT_CORE_ASSERT(success, "Could not intialize GLFW!");
			//If Initalized successfully but get the wrong on run time
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		//��ʼ��Windows���󲢴�������������
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		//ͨ��glad����OpenGL�ṩ�ĸ���ͼ����Ⱦ����
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NUT_CORE_ASSERT(status, "Failed to initialize Glad!");

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//------------------Set GLFW callbacks FN(function) in function "init()"----------------------
		glfwSetWindowSizeCallback(m_Window,
			[](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			}
		);																//lambda���ʽ��Ϊ�ڶ�������

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);									//data.EventCallback�д�����OnEvent������event��Ϊ�����ʹ��
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS: {KeyPressedEvent event(key, 0);	data.EventCallback(event);	break; }
				case GLFW_RELEASE: {KeyReleasedEvent event(key);	data.EventCallback(event);	break; }
				case GLFW_REPEAT: {KeyPressedEvent event(key, 1);	data.EventCallback(event);	break; }
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS: { MouseButtonPressedEvent event(button);	data.EventCallback(event);	break; }
				case GLFW_RELEASE: {MouseButtonReleasedEvent event(button);	data.EventCallback(event);	break; }
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xpos, (float)ypos);
				data.EventCallback(event);
			});
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

	void WindowsWindow::SetVSync(bool enabled)							//�Ƿ����ô�ֱͬ��
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