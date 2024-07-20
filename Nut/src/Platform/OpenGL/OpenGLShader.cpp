#include "nutpch.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <filesystem>

namespace Nut
{
	GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") {
			return GL_VERTEX_SHADER;
		}
		else if (type == "fragment" || type == "pixel") {
			return GL_FRAGMENT_SHADER;
		}

		NUT_CORE_ASSERT(false, "Unknown shader type!")
			return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		NUT_PROFILE_FUNCTION();

		// Get Shader code
		std::string source = ReadFile(filepath);
		std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source);
		Compile(shaderSources);
		// Get Shader's name though filepath name
		auto lastSlash = filepath.find_last_of("/\\");					// maybe : 1.assets/textures/shader.glsl		2./shader	3.shader.glsl	4.shader
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		lastDot = lastDot == std::string::npos ? filepath.size() : lastDot;
		auto count = lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
		// But we can use "filesystem" to simplify the syntax (And for "../shader.glsl", it'll be safer)
		//std::filesystem::path path = filepath;
		//m_Name = path.stem().string();
	};

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		NUT_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(shaderSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		NUT_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		NUT_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		NUT_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	#pragma region 对着色器文本文件glsl的处理函数定义
	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		NUT_PROFILE_FUNCTION();

		std::string result;
		std::ifstream readIn(filepath, std::ios::in | std::ios::binary);
		if (readIn)															//是否成功打开
		{
			readIn.seekg(0, std::ios::end);
			size_t size = readIn.tellg();

			if (size != -1) {												//是否成功获取
				result.resize(size);
				readIn.seekg(0, std::ios::beg);
				readIn.read(&result[0], size);
				readIn.close();
			}
			else {
				NUT_CORE_ERROR("Failed to read file from : '{0}'", filepath);
			}
		}
		else {
			NUT_CORE_ERROR("Could not open file form : '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		NUT_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* token = "#type";
		size_t tokenLength = strlen(token);
		size_t pos = source.find(token, 0);

		while (pos != std::string::npos)
		{
			size_t begin = pos + tokenLength + 1;
			size_t end_of_line = source.find_first_of("\r\n", pos);
			NUT_CORE_ASSERT((end_of_line != std::string::npos), "Syntax error");
			std::string type = source.substr(begin, end_of_line - begin);
			NUT_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", end_of_line);
			pos = source.find(token, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] =
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}
	#pragma endregion

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		NUT_PROFILE_FUNCTION();

		m_RendererID = glCreateProgram();
		NUT_CORE_ASSERT((shaderSources.size() <= 2), "We only support 2 shaders for now ! ");
		int glShaderIndex = 0;
		std::array<GLuint, 2> glShaders;

		for (auto& key_value : shaderSources) {
			GLenum type = key_value.first;
			const std::string& source = key_value.second;
			// ---------------------------------
			GLuint shader = glCreateShader(type);

			const GLchar* sourceC_Str = source.c_str();
			glShaderSource(shader, 1, &sourceC_Str, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				NUT_CORE_ERROR("{0}", infoLog.data());
				NUT_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(m_RendererID, shader);
			glShaders[glShaderIndex++] = shader;
		}

		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(m_RendererID);

			for (auto shader : glShaders)
				glDeleteShader(shader);

			NUT_CORE_ERROR("{0}", infoLog.data());
			NUT_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto shader : glShaders) {
			glDetachShader(m_RendererID, shader);
			glDeleteShader(shader);
		}
	}
	// -----------------------------Overrided from Shader(Parent class)--------------------------------------
	void OpenGLShader::SetInt(const std::string& name, const int& value)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}
	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformIntArray(name, values, count);
	}
	void OpenGLShader::SetFloat(const std::string& name, const float& value)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}
	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		UploadUniformMat3(name, value);
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		NUT_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}
	// -----------------------------------Native in Subclasses-------------------------------------------------
	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value)
	{
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}