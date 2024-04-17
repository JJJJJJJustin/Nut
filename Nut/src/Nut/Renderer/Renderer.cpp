#include "nutpch.h"
#include "Renderer.h"

namespace Nut
{
	
	RendererAPI Renderer::s_API = RendererAPI::OpenGL;					//静态变量需要 1.在类外 2.被初始化

}