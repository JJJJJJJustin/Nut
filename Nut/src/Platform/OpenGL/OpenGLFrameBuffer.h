#pragma once

#include "Nut/Renderer/FrameBuffer.h"

namespace Nut
{

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Recreate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };
		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; };
	private:
		uint32_t m_RendererID = 0;

		uint32_t m_ColorAttachment = 0;
		uint32_t m_BufferAttachment = 0;					// °üÀ¨: DepthAttachment, StencilAttachment ...
		FrameBufferSpecification m_Specification;
	};

}