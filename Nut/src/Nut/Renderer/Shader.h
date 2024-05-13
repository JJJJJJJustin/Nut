#pragma once

#include <string>

#include <unordered_map>
#include <glm/glm.hpp>

namespace Nut
{
	class Shader
	{
	public:
		virtual ~Shader() = default;						//表示编译器对析构函数生成默认实现

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary										//类名：着色器库。
	{														//因为此类抽象度较高，故不用在 OpenGLShader 中进行设置，可以直接在 Shader.h 中进行定义。（不涉及关于某一个接口如 OpenGL/DirectX..的细节）
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string name, const Ref<Shader>& shader);

		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}