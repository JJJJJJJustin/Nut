#include "nutpch.h"
#include "Platform/Windows/WindowsInput.h"

#include "Nut\Core\Application.h"

#include <GLFW\glfw3.h>

namespace Nut
{
	bool WindowsInput::IsKeyPressedImpl(int keycode)					//是否按下？（按下和重复都算做是按下了）
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//.Get()返回Application这个单例中的*s_Instance实例 .GetWindow()返回Application类中的成员m_Window（Windows类的） .GetNativeWindow()返回Window类中的成员GLFWwindow
		
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;				//返回值需要是bool
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
		glfwGetCursorPos(window, &xpos, &ypos);							//glfwGetCursorPos( )函数会自动为xpos和ypos赋值
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