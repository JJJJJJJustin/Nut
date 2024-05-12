#pragma once

#include "nutpch.h"

#include "Core.h"
#include "Nut/Events/Event.h"													//??? For what: For the class name Event which bu used in the following lines of code

namespace Nut {
	//windows properties（窗口属性）
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Nut Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			:Title(title), Width(width), Height(height) {}						//Default（默认情况）
	};


	//An interface for the desktop system which based on Window（基于Windows的桌面系统的接口）
	class NUT_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;					//??? For what
		//-----------------------All interfaces----------------------------------
		static Window* Create(const WindowProps& props = WindowProps());		//返回指向Windows类型的指针

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		//Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;								//Vertical Sync（垂直同步）
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
	};
}