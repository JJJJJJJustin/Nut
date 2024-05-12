#pragma once

#include "nutpch.h"

#include "Core.h"
#include "Nut/Events/Event.h"													//??? For what: For the class name Event which bu used in the following lines of code

namespace Nut {
	//windows properties���������ԣ�
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Nut Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			:Title(title), Width(width), Height(height) {}						//Default��Ĭ�������
	};


	//An interface for the desktop system which based on Window������Windows������ϵͳ�Ľӿڣ�
	class NUT_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;					//??? For what
		//-----------------------All interfaces----------------------------------
		static Window* Create(const WindowProps& props = WindowProps());		//����ָ��Windows���͵�ָ��

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		//Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;								//Vertical Sync����ֱͬ����
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
	};
}