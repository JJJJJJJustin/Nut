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
	//���ԣ����x��ʾ������������У�{0}ռλ��"__VA_ARGS__"����"..."���������䣩
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

//��1�ڶ�������������xλ: Bit(2)->00000100
#define Bit(x) (1 << x)

#define NUT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)