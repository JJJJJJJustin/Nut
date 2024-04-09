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
		uint32_t m_RendererID;								// uint32_t �� 32 λ�޷������ͣ��� unsigned int ����ƽ̨������������ͬ
	};

}