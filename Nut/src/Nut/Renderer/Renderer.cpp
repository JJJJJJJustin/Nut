#include "nutpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthoGraphicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UpdateUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);						//����Ĳ�����OpenGLShader���͵ģ�����Ҫ�ֶ�ת����
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UpdateUniformMat4("u_Transform", transform);
		
		vertexArray->Bind();
		RendererCommand::DrawIndexed(vertexArray);
	}

}