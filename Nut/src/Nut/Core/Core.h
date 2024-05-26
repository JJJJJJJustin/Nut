#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define NUT_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__ANDROID__)						/* We also have to check __ANDROID__ before __linux__.  Since android is based on the linux kernel , and it has __linux__ defined */
	#define NUT_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__Linux__)
	#define NUT_PLATFORM_LINUX
	#error "Linux is not supported!"
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>				// Apple �кܶ�����ʽ��������Ҫȷ����Ӧ��ƽ̨�������ж�Ӧ�Ķ���
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS Simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define NUT_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC
		#define NUT_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
#else
	#error "Unknown platform!"
#endif

#ifdef NUT_PLATFORM_WINDOWS
	#ifdef NUT_DYNAMIC_LINK
		#ifdef NUT_BUILD_DLL
			#define NUT_API __declspec(dllexport)
		#else
			#define NUT_API __declspec(dllimport)
		#endif
	#else
		#define NUT_API
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
			NUT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);\
			__debugbreak();}\
		}
	#define NUT_ASSERT(x, ...)\
		{if(!x){\
			NUT_ERROR("Assertion Failed: {0}", __VA_ARGS__);\
			__debugbreak();}\
		}
#else
	#define NUT_CORE_ASSERT(x, ...)
	#define NUT_ASSERT(x, ...)
#endif

//��1�ڶ�������������xλ: Bit(2)->00000100
#define Bit(x) (1 << x)

#define NUT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Nut 
{
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>( std::forward<Args>(args)... );									// make_shared �Ĳ����� ����CreateRef������� ����̬�����Ĳ�������Щ����CreateRef�е�ģ������ᱻ std::forward ������ת��
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>( std::forward<Args>(args)... );
	}

}