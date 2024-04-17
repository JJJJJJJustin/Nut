#pragma once

#include "Nut/Renderer/Buffer.h"

#include <memory>

namespace Nut
{
	
	class VertexArray 
	{
	public:
		virtual ~VertexArray() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// For an vertex buffer it should have one index buffer usually, cuz vertex buffer can include so much more data such as pos/color... ,but it all rendering one object
		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexbuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexbuffer) = 0;	

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer () const = 0;

		static VertexArray* Create();

	};

}