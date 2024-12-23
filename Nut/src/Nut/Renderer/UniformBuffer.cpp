#include "nutpch.h"
#include "Nut/Renderer/UniformBuffer.h"

#include "Nut/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Nut
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size, binding);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;

	}

}
