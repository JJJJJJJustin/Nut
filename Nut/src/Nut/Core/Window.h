#pragma once

#include "nutpch.h"

#include "Nut/Core/Base.h"
#include "Nut/Events/Event.h"													//??? For what: For the class name Event which bu used in the following lines of code

namespace Nut {
	//windows properties（窗口属性）
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Nut Engine",
			uint32_t width = 1600,
			uint32_t height = 900)
			:Title(title), Width(width), Height(height) {}						//Default（默认情况）
	};


	//An interface for the desktop system which based on Window（基于Windows的桌面系统的接口）
	class Window
	{
	public:
		static Scope<Window> Create(const WindowProps& props = WindowProps());		//返回指向Windows类型的指针
		//-----------------------All interfaces----------------------------------
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		//Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;								//Vertical Sync（垂直同步）
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
	};
}