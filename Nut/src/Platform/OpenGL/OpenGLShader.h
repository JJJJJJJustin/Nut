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
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		const std::string& GetName() const override { return m_Name; }

		void SetInt   (const std::string& name, const int& value) override;
		void SetFloat (const std::string& name, const float& value) override;
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetFloat4(const std::string& name, const glm::vec4& value) override;
		void SetMat3  (const std::string& name, const glm::mat3& value) override;
		void SetMat4  (const std::string& name, const glm::mat4& value) override;

		void UploadUniformInt   (const std::string& name, const int& value);
		void UploadUniformFloat (const std::string& name, const float& value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat3  (const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4  (const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);										//将文件读入一个字符串中并返回以供使用
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSource);
	private:
		uint32_t m_RendererID;								// uint32_t 是 32 位无符号整型，而 unsigned int 根据平台编译器有所不同
		std::string m_Name;
	};

}