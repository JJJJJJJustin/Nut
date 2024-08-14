#include "nutpch.h"
#include "Nut/Core/Input.h"

#include "Nut\Core\Application.h"

#include <GLFW\glfw3.h>

namespace Nut
{
	bool Input::IsKeyPressed(int keycode)					//�Ƿ��£������º��ظ��������ǰ����ˣ�
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//.Get()����Application��������е�*s_Instanceʵ�� .GetWindow()����Application���еĳ�Աm_Window��Windows��ģ� .GetNativeWindow()����Window���еĳ�ԱGLFWwindow
		
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;				//����ֵ��Ҫ��bool
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float,float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);							//glfwGetCursorPos( )�������Զ�Ϊxpos��ypos��ֵ
		return{ (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}
}