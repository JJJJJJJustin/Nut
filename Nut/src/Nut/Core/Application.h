#pragma once

#include "Nut/Core/Base.h"

#include "Nut/Events/Event.h"
#include "Nut/Events/ApplicationEvent.h"

#include "Nut/Core/Window.h"
#include "Nut/Core/LayerStack.h"
#include "Nut/ImGui/ImGuiLayer.h"

#include "Nut/Core/Timestep.h"

int main(int argc, char** argv);

namespace Nut {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			NUT_CORE_ASSERT((index < Count), "Application Command Line Arags invalid");
			return Args[index];
		}
	};


	class Application
	{
	public:
		Application(const std::string& name = "Nut App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void OnEvent(Event& e);							//事件分发

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }			//返回下面这个指向Window的指针
		inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		inline static Application& Get() { return *s_Instance; }	// ! ! ! 返回的是 s_Instance 这个指向 Application 的指针
																	//（为什么函数是引用传递？：因为application是一个单例，如果不使用&则会多出一个复制，这有悖于单例模式的只有一个对象的要求
		inline ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
		
		inline void WindowClose() { m_Running = false; }			// 不同于 OnWindowClose ，WindowClose 是公有的关闭窗口函数，而 OnWindowClose 是回调函数中的事件处理
	private:
		void Run();													// Run 函数现在为私有（ Run 函数中定义 RunLoop）

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;

		bool m_Running = true;
		bool m_Minimized = false;

		std::unique_ptr<Window> m_Window;				//指向Window的指针
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;					//! ! !唯一实例的静态成员（static类型，需要初始化定义）
		friend int ::main(int argc, char** argv);		// 通过将 main 声明为友元函数，便可以在外部通过 main 来访问私有的 Run 函数
	};

	//To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);

}