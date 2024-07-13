#pragma once

#include "Nut/Renderer/RendererCommand.h"

#include "Nut/Renderer/OrthoGraphicCamera.h"
#include "Nut/Renderer/Shader.h"

namespace Nut
{

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthoGraphicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }			//ע�⾲̬����ʹ��ʱ���������ʶ
	private:
		struct SceneData{
			glm::mat4 ViewProjectionMatrix;
		};
		static Scope<SceneData> s_SceneData;
	};
}