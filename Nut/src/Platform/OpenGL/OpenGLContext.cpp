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
			//通过glad加载OpenGL提供的各种图形渲染函数
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NUT_CORE_ASSERT(status, "Failed to initialize Glad!");

		NUT_CORE_INFO("OpenGL info: ");
		NUT_CORE_INFO("Vendor: {0}",	(const char*)glGetString(GL_VENDOR));
		NUT_CORE_INFO("Renderer: {0}",	(const char*)glGetString(GL_RENDERER));
		NUT_CORE_INFO("Version: {0}",	(const char*)glGetString(GL_VERSION));

		#ifdef NUT_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
			NUT_CORE_ASSERT( (versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5)), "Nut requires at least OpenGL Version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}