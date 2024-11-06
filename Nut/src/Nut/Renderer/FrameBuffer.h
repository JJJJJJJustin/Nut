#pragma once

namespace Nut
{
	/*struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};*/

	enum class FrameBufferAttachmentFormat			// The formats of attachment
	{
		None = 0,

		// Color Attachment
		RGBA8,
		RED_INTEGER,

		// Depth / Stencil Attachment
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8

	};

	struct FrameBufferAttachmentsSpecification
	{
		FrameBufferAttachmentsSpecification() = default;
		FrameBufferAttachmentsSpecification(std::initializer_list<FrameBufferAttachmentFormat> attachments)
			: Attachments(attachments) {}

		std::vector<FrameBufferAttachmentFormat> Attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		FrameBufferAttachmentsSpecification Attachments;
		//bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;

	};


}