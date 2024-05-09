#include "nutpch.h"
#include "Shader.h"

#include "Nut/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut
{

	Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(filepath);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}


	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSrc, fragmentSrc);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}

}