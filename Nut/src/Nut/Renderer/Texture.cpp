#include "nutpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Nut
{
		Ref<Texture2D> Texture2D::Create(const std::string& path)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
				return nullptr;
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLTexture2D>(path);
			case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
				return nullptr;
			}

			NUT_CORE_ASSERT(false, "Unknown Renderer API!")
				return nullptr;
		}

}