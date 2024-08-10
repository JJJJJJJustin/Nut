#pragma once

#include "Nut/Renderer/FrameBuffer.h"

namespace Nut
{

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Resize();

		void Bind() override;
		void Unbind() override;

		uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };
		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; };
	private:
		uint32_t m_RendererID;

		uint32_t m_ColorAttachment;
		uint32_t m_BufferAttachment;					// °üÀ¨: DepthAttachment, StencilAttachment ...
		FrameBufferSpecification m_Specification;
	};

}