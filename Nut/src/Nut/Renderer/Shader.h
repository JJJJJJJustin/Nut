#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Nut
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UpdateUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;								// uint32_t 是 32 位无符号整型，而 unsigned int 根据平台编译器有所不同
	};

}