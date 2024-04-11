#pragma once

namespace Nut
{

	enum class RendererAPI
	{
		None = 0, OpenGL = 1, DirectX = 2
	};

	class Renderer
	{
	public:
		static inline RendererAPI SetAPI(RendererAPI api) { s_API = api; }
		static inline RendererAPI GetAPI() { return s_API; }			//注意静态函数使用时的作用域标识
	private:
		static RendererAPI s_API;
	};

	RendererAPI Renderer::s_API = RendererAPI::OpenGL;					//静态变量需要 1.在类外 2.被初始化

}