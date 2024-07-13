#include "nutpch.h"
#include "Nut/Renderer/Renderer.h"
#include "Nut/Renderer/Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		NUT_PROFILE_FUNCTION();

		RendererCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()						//????? why create it ?
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RendererCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthoGraphicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);						//传入的参数是OpenGLShader类型的，故需要手动转换。
		shader->SetMat4("u_Transform", transform);
		
		vertexArray->Bind();
		RendererCommand::DrawIndexed(vertexArray);
	}

}