#include "nutpch.h"
#include "Renderer.h"

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

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->UpdateUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->UpdateUniformMat4("u_Transform", transform);
		
		vertexArray->Bind();
		RendererCommand::DrawIndexed(vertexArray);
	}

}