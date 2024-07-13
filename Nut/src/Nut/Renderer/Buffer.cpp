#include "nutpch.h"
#include "Nut/Renderer/Buffer.h"

#include "Nut/Renderer/RendererAPI.h"											// For choosing suitable API

#include "Platform/OpenGL/OpenGLBuffer.h"						// For using subclass's constructor which is wrote by an API you want to
//#include "Platform/DirectX/DirtectXBuffer.h"					// So that initialize buffer as you liked

namespace Nut
{
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI()) 
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ") 
								return nullptr;
		case RendererAPI::API::OpenGL: 
								return CreateRef<OpenGLVertexBuffer>(vertices, size);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
								return nullptr;
		}
		
		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
								return nullptr;
		case RendererAPI::API::OpenGL:
								return CreateRef<OpenGLIndexBuffer>(indices, count);
		case RendererAPI::API::DirectX:	NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
								return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}
}