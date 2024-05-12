#include "nutpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Nut
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexbuffer)
	{
		NUT_CORE_ASSERT(vertexbuffer->GetLayout().GetElements().size(), " The Vertex Buffer Layout now is empty ! ( Fix it if you don't set or not use OpenGLVertexBuffer::SetLayout())")
		// Bind VA & VB
		glBindVertexArray(m_RendererID);
		vertexbuffer->Bind();
		// Vertex Attrib 
		const auto& layout = vertexbuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex, element.Count, element.GLType,
				element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(intptr_t)element.Offset);		//(intptr_t) 是一种足够大的整数类型，先将Offset转换成了整数型。
			m_VertexBufferIndex++;
		}

		m_VertexBuffers.push_back(vertexbuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer)
	{
		// Bind VA & IB
		glBindVertexArray(m_RendererID);
		indexbuffer->Bind();

		m_IndexBuffer = indexbuffer;
	}

}