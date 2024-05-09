#pragma once

#include "Nut/Renderer/Shader.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Nut
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void UpdateUniformInt(const std::string& name, const int& value);

		void UpdateUniformFloat(const std::string& name, const float& value);
		void UpdateUniformFloat2(const std::string& name, const glm::vec2& value);
		void UpdateUniformFloat3(const std::string& name, const glm::vec3& value);
		void UpdateUniformFloat4(const std::string& name, const glm::vec4& value);

		void UpdateUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UpdateUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);										//将文件读入一个字符串中并返回以供使用
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSource);
	private:
		uint32_t m_RendererID;								// uint32_t 是 32 位无符号整型，而 unsigned int 根据平台编译器有所不同
	};

}