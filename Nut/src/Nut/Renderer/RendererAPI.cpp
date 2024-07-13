#include "nutpch.h"

#include "Nut/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace Nut
{

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;						//∂®“Â s_API

	Scope<RendererAPI> RendererAPI::Create() {
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}
		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}

}
 