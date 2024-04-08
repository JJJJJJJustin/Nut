#include "nutpch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>											// ---- glad is OpenGL specific ----

namespace Nut
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		NUT_CORE_ASSERT(windowHandle,"Window handle is null ! ( But it shouldn't be )")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
			//ͨ��glad����OpenGL�ṩ�ĸ���ͼ����Ⱦ����
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NUT_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}