#include "nutpch.h"
#include "Nut/Core/Input.h"

#ifdef NUT_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
#endif

namespace Nut {

	Scope<Input> Input::s_Instance = Input::Create();						//s_Instance ��һ��û�а����κ����ݵ�ʵ������

	Scope<Input> Input::Create() 
	{
	#ifdef NUT_PLATFORM_WINDOWS
			return CreateScope<WindowsInput>();
	#elif defined NUT_PLATFORM_MACOS
			return CreateScope<MacosInput>();
	#elif defined NUT_PLATFORM_LINUX
			return CreateScope<LinuxInput>();
	#else
			NUT_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
	#endif
	}

}