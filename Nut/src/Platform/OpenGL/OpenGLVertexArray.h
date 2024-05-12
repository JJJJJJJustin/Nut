#pragma once

#include "Nut/Renderer/VertexArray.h"


namespace Nut
{
	
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray() ;
		virtual ~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexbuffer) override;
		void SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer) override;

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}