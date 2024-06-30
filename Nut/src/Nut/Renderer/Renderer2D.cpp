#include "nutpch.h"
#include "Renderer2D.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "RendererCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVA;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};
	static Renderer2DStorage* s_Data;														// 为什么是指针：方便管理内存的分配与释放


	void Renderer2D::Init()
	{
		NUT_PROFILE_FUNCTION();

		s_Data = new Renderer2DStorage();

		float squareVertices[5 * 4] =
		{
			-0.5f, -0.5f, -0.1f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.1f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.1f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.1f, 0.0f, 1.0f
		};
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

		BufferLayout squareLayout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareLayout);

		s_Data->QuadVA = VertexArray::Create();
		s_Data->QuadVA->AddVertexBuffer(squareVB);
		s_Data->QuadVA->SetIndexBuffer(squareIB);

		// Shader
		s_Data->TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");			//根据glsl创建着色器对象
		s_Data->TextureShader->Bind();															//绑定着色器对象
		s_Data->TextureShader->SetInt("u_Texture", 0);											//将要采样的图片设置在0号纹理单元中，设置为0号之后，之后绑定的纹理会被渲染在0号图层中。（纹理单元允许同一个位置同时渲染多个图层的纹理）

		// Texture
		s_Data->WhiteTexture = Texture2D::Create(1,1);											//通过Create函数设置宽高比，根据包含颜色数据设置内存，直接从底层创建白色纹理
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
	}

	void Renderer2D::Shutdown()
	{
		NUT_PROFILE_FUNCTION();

		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthoGraphicCamera& camera)
	{
		NUT_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
		NUT_PROFILE_FUNCTION();

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->WhiteTexture->Bind();

		s_Data->QuadVA->Bind();
		RendererCommand::DrawIndexed(s_Data->QuadVA);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y }, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetFloat4("u_Color", glm::vec4{ 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		texture->Bind();														//将纹理采样至0号纹理单元

		s_Data->QuadVA->Bind();
		RendererCommand::DrawIndexed(s_Data->QuadVA);
	}

}