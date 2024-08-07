#pragma once

#include "Nut\Core\Base.h"

namespace Nut
{
	class Input 
	{
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;									//对该类对象禁用复制（复制函数）
		Input& operator=(const Input&) = delete;						//对该类对象禁用赋值操作

		static Scope<Input> Create();

		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;					//纯虚
		
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;			//C++17
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Scope<Input> s_Instance;										//在 Input.cpp 中初始化（静态成员变量必须被定义或初始化）
	};
}