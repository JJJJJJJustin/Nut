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