#include "nutpch.h"
#include "VertexArray.h"

#include "Nut/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Nut
{

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLVertexArray();
			case RendererAPI::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
				return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
		return nullptr;
	}

}