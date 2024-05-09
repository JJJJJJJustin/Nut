#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Nut
{
	class Shader
	{
	public:
		virtual ~Shader() = default;						//��ʾ��������������������Ĭ��ʵ��

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& filepath);
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}