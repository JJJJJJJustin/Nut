#pragma once

#include "Nut/Renderer/RendererAPI.h"

namespace Nut
{

	class RendererCommand
	{
	public:
		// static variable from Create(), which returns virtual funcs already overrided in Platform
		// So bind the functions form corresponding interface in OpenGL/DirectX/Metal/Valkan
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void Clear() 
		{
			s_RendererAPI->Clear();
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
	
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)							//GL_STATIC_DRAW
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)	//GL_DYNAMIC_DRAW(with batch rendering)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}