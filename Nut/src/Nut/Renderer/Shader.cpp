#include "nutpch.h"
#include "Nut/Renderer/Shader.h"

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
			return CreateRef<OpenGLShader>(filepath);
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
			return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererAPI::API::DirectX: NUT_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported! ")
			return nullptr;
		}

		NUT_CORE_ASSERT(false, "Unknown Renderer API!")
			return nullptr;
	}


	// ---------------------------------------------------------------------------------------------------
	//ShaderLibrary::ShaderLibrary()
	//{
	//	m_Shaders = std::unordered_map<std::string, Ref<Shader>>();
	//}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();										// GetName��m_Name �� OpenGLShader() ������ 1.��һ�����캯���У�ͨ���ļ�������ȡ 2.����һ�����캯����������ͨ��������ȡ
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		NUT_CORE_ASSERT(!Exists(name), "Shader is already exists!");		// ����ظ� Add ͬһ����ɫ���򱨴�
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

	bool ShaderLibrary::Exists(const std::string& name)	const							// name ������ m_Shaders �з��� true������ false
	{
		if (m_Shaders.empty()) {
			return false;
		}
		
		return m_Shaders.find(name) != m_Shaders.end();
	}

}