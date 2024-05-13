#pragma once

#include <string>

#include <unordered_map>
#include <glm/glm.hpp>

namespace Nut
{
	class Shader
	{
	public:
		virtual ~Shader() = default;						//��ʾ��������������������Ĭ��ʵ��

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary										//��������ɫ���⡣
	{														//��Ϊ�������Ƚϸߣ��ʲ����� OpenGLShader �н������ã�����ֱ���� Shader.h �н��ж��塣�����漰����ĳһ���ӿ��� OpenGL/DirectX..��ϸ�ڣ�
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