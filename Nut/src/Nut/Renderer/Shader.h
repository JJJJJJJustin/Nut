#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Nut
{
	class Shader
	{
	public:
		virtual ~Shader() = default;						//表示编译器对析构函数生成默认实现

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& filepath);
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}