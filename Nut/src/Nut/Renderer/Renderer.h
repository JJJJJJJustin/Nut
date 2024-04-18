#pragma once

#include "RendererCommand.h"

namespace Nut
{

	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }			//注意静态函数使用时的作用域标识
	private:
	};

}