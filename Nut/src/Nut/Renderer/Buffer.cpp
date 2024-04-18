#include "nutpch.h"
#include "Buffer.h"

#include "RendererAPI.h"											// For choosing suitable API

#include "Platform/OpenGL/OpenGLBuffer.h"						// For using subclass's constructor which is wrote by an API you want to
//#include "Platform/DirectX/DirtectXBuffer.h"					// So that initialize buffer as you liked

namespace Nut
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI()) 
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ") 
								return nullptr;
		case RendererAPI::API::OpenGL: 
								return new OpenGLVertexBuffer(vertices, size);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
								return nullptr;
		}
		
		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
								return nullptr;
		case RendererAPI::API::OpenGL:
								return new OpenGLIndexBuffer(indices, count);
		case RendererAPI::API::DirectX:	NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
								return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}
}