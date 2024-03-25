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

#ifdef NUT_DEBUG
	#define NUT_ENABLE_ASSERTS
#endif

#ifdef NUT_ENABLE_ASSERTS
	//断言（如果x表示错误则语句运行，{0}占位的"__VA_ARGS__"代表"..."所输入的语句）
	#define NUT_CORE_ASSERT(x, ...) \
		{if(!x){\
			NUT_CORE_ERROR("Assertion Failed: {0}, __VA_ARGS__");\
			__debugbreak();}\
		}
	#define NUT_ASSERT(x, ...)\
		{if(!x){\
			NUT_ERROR("Assertion Failed: {0}, __VA_ARGS__");\
			__debugbreak();}\
		}
#else
	#define NUT_CORE_ASSERT(x, ...)
	#define NUT_ASSERT(x, ...)
#endif

//将1在二进制数中左移x位: Bit(2)->00000100
#define Bit(x) (1 << x)

#define NUT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)