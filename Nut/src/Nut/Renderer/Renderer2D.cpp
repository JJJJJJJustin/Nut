#include "nutpch.h"
#include "Nut/Renderer/Renderer2D.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Nut/Renderer/Shader.h"
#include "Nut/Renderer/VertexArray.h"
#include "Nut/Renderer/RendererCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Nut {

	struct QuadVertex 
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		// TODO:TexId
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 100;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices  = MaxQuads * 6;
		uint32_t QuadIndexCount = 0;

		Ref<VertexArray> QuadVA;
		Ref<VertexBuffer> QuadVB;
		Ref<IndexBuffer> QuadIB;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		QuadVertex* QuadVBBase = nullptr;												// 顶点指针起始
		QuadVertex* QuadVBHind = nullptr;												// 顶点指针末尾
	};
	static Renderer2DData s_Data;														// 更改为栈上分配


	void Renderer2D::Init()
	{
		NUT_PROFILE_FUNCTION();

		// VertexBuffer + IndexBuffer + VertexArray
		s_Data.QuadVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		BufferLayout squareLayout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};
		s_Data.QuadVB->SetLayout(squareLayout);
		
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		s_Data.QuadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		delete[] quadIndices;

		s_Data.QuadVA = VertexArray::Create();
		s_Data.QuadVA->SetIndexBuffer(s_Data.QuadIB);
		s_Data.QuadVA->AddVertexBuffer(s_Data.QuadVB);

		// QuadVertex Ptr
		s_Data.QuadVBBase = new QuadVertex[s_Data.MaxVertices];									//保存指针初始位置

		// Shader
		s_Data.TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");				//根据glsl创建着色器对象
		s_Data.TextureShader->Bind();															//绑定着色器对象
		s_Data.TextureShader->SetInt("u_Texture", 0);											//将要采样的图片设置在0号纹理单元中，设置为0号之后，之后绑定的纹理会被渲染在0号图层中。（纹理单元允许同一个位置同时渲染多个图层的纹理）

		// Texture
		s_Data.WhiteTexture = Texture2D::Create(1,1);											//通过Create函数设置宽高比，根据包含颜色数据设置内存，直接从底层创建白色纹理
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
	}

	void Renderer2D::Shutdown()
	{
		NUT_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthoGraphicCamera& camera)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;																//每结束（刷新）一次批渲染，需要绘制的索引数要从零重新开始
		s_Data.QuadVBHind = s_Data.QuadVBBase;													//初始化后端指针 Hind 的位置（最初为零）
	}

	void Renderer2D::EndScene()
	{
		NUT_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVBHind - (uint8_t*)s_Data.QuadVBBase;			// Size 等于后端指针减去前端(hind 在绘制时一直更新数据）
		s_Data.QuadVB->SetData(s_Data.QuadVBBase, dataSize);									// Reset VertexBuffer so flush Vertex data （because of Dynamic Draw)

		Flush();																				// 更新数据之后绘制（刷新）
	}

	void Renderer2D::Flush()
	{
		RendererCommand::DrawIndexed(s_Data.QuadVA, s_Data.QuadIndexCount);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.QuadVBHind->Position = position;
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x,			position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBHind++;

		s_Data.QuadIndexCount += 6;

#if 0
		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }
		);
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetMat4("u_Transform", transform);
		s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);

		s_Data.QuadVA->Bind();
		s_Data.WhiteTexture->Bind();
#endif

		RendererCommand::DrawIndexed(s_Data.QuadVA);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		NUT_PROFILE_FUNCTION();

#if 0
		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }
		);
		s_Data.TextureShader->SetMat4("u_Transform", transform);
		s_Data.TextureShader->SetFloat4("u_Color", tintColor);
		s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);

		s_Data.QuadVA->Bind();
		texture->Bind();														//将纹理采样至0号纹理单元
#endif

		RendererCommand::DrawIndexed(s_Data.QuadVA);
	}
	//------------------------------------------------- Rotated Quad --------------------------------------------------------------
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }
		);
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetMat4("u_Transform", transform);
		s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);

		s_Data.QuadVA->Bind();
		s_Data.WhiteTexture->Bind();

		RendererCommand::DrawIndexed(s_Data.QuadVA);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }
		);
		s_Data.TextureShader->SetMat4("u_Transform", transform);
		s_Data.TextureShader->SetFloat4("u_Color", tintColor);
		s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);

		s_Data.QuadVA->Bind();
		texture->Bind();

		RendererCommand::DrawIndexed(s_Data.QuadVA);
	}

}