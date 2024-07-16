#include "nutpch.h"
#include "Nut/Core/Window.h"

#ifdef NUT_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Nut {

	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef NUT_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#elif defined NUT_PLATFORM_MACOS
			return CreateScope<MacosWindow>(props);
		#elif defined NUT_PLATFORM_LINUX
			return CreateScope<LinuxWindow>(props);
		#else
			NUT_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}


}