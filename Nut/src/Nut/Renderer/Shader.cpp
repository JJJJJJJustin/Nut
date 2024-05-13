#include "nutpch.h"
#include "Shader.h"

#include "Nut/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut
{

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}


	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NUT_CORE_ASSERT(false, "RendererAPI::None is currently not supported! ")
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}


	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->GetName();									// GetName的m_Name 在 OpenGLShader() 中设置 1.在一个构造函数中，通过文件名来截取 2.在另一个构造函数的重载中通过参数获取
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string name, const Ref<Shader>& shader)
	{
		NUT_CORE_ASSERT(!Exists(name), "Shader:'{0}' is already exists!", name);		// 如果重复 Add 同一个着色器则报错
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		NUT_CORE_ASSERT(Exists(name), "Shader not found!")
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)									// name 存在于 m_Shaders 中返回 true，否则 false
	{
		return (m_Shaders.find(name) != m_Shaders.end());
	}

}