#pragma once

#ifdef NUT_PLATFORM_WINDOWS
	#ifdef NUT_BUILD_DLL
		#define NUT_API __declspec(dllexport)
	#else
		#define NUT_API __declspec(dllimport)
	#endif
#else
	#error Nut only supports Windows!
#endif

//将1在二进制数中左移x位: Bit(2)->00000100
#define Bit(x) (1 << x)