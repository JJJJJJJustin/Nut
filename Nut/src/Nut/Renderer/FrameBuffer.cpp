#include "nutpch.h"

#include "Nut/Renderer/Renderer.h"
#include "Nut/Renderer/Framebuffer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Nut
{

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(spec);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}