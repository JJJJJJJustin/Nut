#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Nut
{

	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, DirectX = 2
		};
	public:
		static RendererAPI* Create();

		virtual void Clear() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }			//注意静态函数使用时的作用域标识
		inline static API SetAPI(API api) { s_API = api; }
	
	private:
		static API s_API;
	};

}
