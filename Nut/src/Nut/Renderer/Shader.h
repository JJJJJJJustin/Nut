#pragma once

#include <string>

namespace Nut
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;								// uint32_t 是 32 位无符号整型，而 unsigned int 根据平台编译器有所不同
	};

}