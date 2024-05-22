#include "nutpch.h"
#include "VertexArray.h"

#include "Nut/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Nut
{

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
				return nullptr;
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLVertexArray>();
			case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
				return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
		return nullptr;
	}

}