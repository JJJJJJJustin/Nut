#include "nutpch.h"
#include "Buffer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Nut
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size){
		return new OpenGLVertexBuffer(vertices, size);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {
		return new OpenGLIndexBuffer(indices, count);
	}
}