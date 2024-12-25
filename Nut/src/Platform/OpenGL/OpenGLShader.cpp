#include "nutpch.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <filesystem>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Nut/Core/Timer.h"

namespace Nut
{

	namespace Utils 
	{
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			NUT_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			NUT_CORE_ASSERT(false, "Unknown shader type!");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			NUT_CORE_ASSERT(false, "Unknown shader type!");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			NUT_CORE_ASSERT(false, "Unknown shader type!");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			NUT_CORE_ASSERT(false, "Unknown shader type!");
			return "";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		NUT_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		// Get Shader code
		std::string source = ReadFile(filepath);
		std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			NUT_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}


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
		
		CompileOrGetVulkanBinaries(shaderSources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
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
			NUT_CORE_ERROR("Could not open file from : '{0}'", filepath);
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
			NUT_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", end_of_line);
			pos = source.find(token, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)] =
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}
	#pragma endregion

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					NUT_CORE_ERROR(module.GetErrorMessage());
					NUT_CORE_ASSERT(false, "CompileOrGetVulkanBinaries runtime error");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					NUT_CORE_ERROR(module.GetErrorMessage());
					NUT_CORE_ASSERT(false, "CompileOrGetOpenGLBinaries runtime error");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			NUT_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		NUT_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		NUT_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		NUT_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		NUT_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			NUT_CORE_TRACE("  {0}", resource.name);
			NUT_CORE_TRACE("    Size = {0}", bufferSize);
			NUT_CORE_TRACE("    Binding = {0}", binding);
			NUT_CORE_TRACE("    Members = {0}", memberCount);
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