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

//��1�ڶ�������������xλ: Bit(2)->00000100
#define Bit(x) (1 << x)