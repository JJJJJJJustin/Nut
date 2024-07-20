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
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 100;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices  = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVA;
		Ref<VertexBuffer> QuadVB;
		Ref<IndexBuffer> QuadIB;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVBBase = nullptr;												// 顶点指针起始
		QuadVertex* QuadVBHind = nullptr;												// 顶点指针末尾

		std::array<Ref<Texture2D>, MaxTextureSlots> Textures;
		uint32_t TextureSoltIndex = 1;													// 0 => WhiteTexture
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
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
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

		// Generate Samplers
		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		// Shader
		s_Data.TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");				//根据glsl创建着色器对象
		s_Data.TextureShader->Bind();															//绑定着色器对象
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);		//上传所有采样器到对应纹理单元

		// Texture
		s_Data.WhiteTexture = Texture2D::Create(1,1);											//通过Create函数设置宽高比，根据包含颜色数据设置内存，直接从底层创建白色纹理
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Data.Textures[0] = s_Data.WhiteTexture;

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

		s_Data.QuadIndexCount = 0;																//每结束（刷新）一次批渲染，需要绘制的顶点索引数要从零重新开始
		s_Data.TextureSoltIndex = 1;															//每结束（刷新）一次批渲染，需要绘制的纹理索引要从一重新开始（排除白色纹理）
		s_Data.QuadVBHind = s_Data.QuadVBBase;													//每结束（刷新）一次批渲染，需要将后端指针 Hind 的位置赋值为起始位置，从新开始
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
		// Bind texture before rendering
		for (uint32_t i = 0; i < s_Data.TextureSoltIndex; i++)
			s_Data.Textures[i]->Bind(i);															// 对数组使用"->"才是对其中对象进行操作，'.'是对数组进行操作。

		RendererCommand::DrawIndexed(s_Data.QuadVA, s_Data.QuadIndexCount);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		const float textureIndex = 0.0f;									// Just use the white texutre
		const float tilingFactor = 1.0f;									// Single color don't need tiling factor

		// 顶点需要被按照线框上的0,1,2,3顶点序号进行逆时针的顺序放置，以便得到正确的绘制结果
		s_Data.QuadVBHind->Position = position;
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x,			position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = color;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
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
		RendererCommand::DrawIndexed(s_Data.QuadVA);
#endif
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		NUT_PROFILE_FUNCTION();
		
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSoltIndex; i++) {				// 遍历纹理，查看现有纹理是否已经存入。若命中，则将i赋予给临时变量textureIndex，并跳出。（这里的每一个纹理的索引可以看做是其编号，通过纹理集中的位置表示:0,1,2 ...）
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// 将纹理在纹理集中的位置作为索引
				break;
			}
		}
		if (textureIndex == 0.0f) {												// 若未命中，则将纹理放入纹理集中，并将最新的s_Data.TextureSoltIndex赋予给临时变量textureIndex，并自增一次
			s_Data.Textures[s_Data.TextureSoltIndex] = texture;
			textureIndex = float(s_Data.TextureSoltIndex);						// 从TextureSoltIndex = 0开始，依次相纹理集中存入新的纹理。存入后自增一次.
			
			s_Data.TextureSoltIndex++;
		}

		// 顶点需要被按照线框上的0,1,2,3顶点序号进行逆时针的顺序放置，以便得到正确的绘制结果
		s_Data.QuadVBHind->Position = position;
		s_Data.QuadVBHind->Color = tintColor;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVBHind->Color = tintColor;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = tintColor;
		s_Data.QuadVBHind->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadVBHind->Position = { position.x,			position.y + size.y, 0.0f };
		s_Data.QuadVBHind->Color = tintColor;
		s_Data.QuadVBHind->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBHind->TexIndex = textureIndex;
		s_Data.QuadVBHind->TilingFactor = tilingFactor;
		s_Data.QuadVBHind++;

		s_Data.QuadIndexCount += 6;

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
		RendererCommand::DrawIndexed(s_Data.QuadVA);
#endif
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