#include "nutpch.h"
#include "Platform/Windows/WindowsInput.h"

#include "Nut\Core\Application.h"

#include <GLFW\glfw3.h>

namespace Nut
{
	bool WindowsInput::IsKeyPressedImpl(int keycode)					//�Ƿ��£������º��ظ��������ǰ����ˣ�
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//.Get()����Application��������е�*s_Instanceʵ�� .GetWindow()����Application���еĳ�Աm_Window��Windows��ģ� .GetNativeWindow()����Window���еĳ�ԱGLFWwindow
		
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;				//����ֵ��Ҫ��bool
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float,float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);							//glfwGetCursorPos( )�������Զ�Ϊxpos��ypos��ֵ
		return{ (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return y;
	}
}