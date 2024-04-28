#pragma once

#include "RendererCommand.h"

#include "OrthoGraphicCamera.h"
#include "Shader.h"

namespace Nut
{

	class Renderer
	{
	public:
		static void Init();

		static void BeginScene(OrthoGraphicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }			//ע�⾲̬����ʹ��ʱ���������ʶ
	private:
		struct SceneData{
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* s_SceneData;
	};
}