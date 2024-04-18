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

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }			//ע�⾲̬����ʹ��ʱ���������ʶ
	private:
	};

}