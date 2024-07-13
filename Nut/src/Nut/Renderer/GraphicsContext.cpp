#include "nutpch.h"
#include "Nut/Renderer/GraphicsContext.h"

#include "Nut/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Nut {

	Scope<GraphicsContext> GraphicsContext::Create(void* window) 
	{
		switch (Renderer::GetAPI()) 
		{
			case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ");
				return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown RendererAPI! ");
		return nullptr;
	}


}