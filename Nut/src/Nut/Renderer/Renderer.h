#pragma once

#include "RendererCommand.h"

#include "OrthoGraphicCamera.h"
#include "Shader.h"

namespace Nut
{

	class Renderer
	{
	public:
		static void BeginScene(OrthoGraphicCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }			//注意静态函数使用时的作用域标识
	private:
		struct SceneData{
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};
}