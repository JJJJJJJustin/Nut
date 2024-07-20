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
		QuadVertex* QuadVBBase = nullptr;												// ����ָ����ʼ
		QuadVertex* QuadVBHind = nullptr;												// ����ָ��ĩβ

		std::array<Ref<Texture2D>, MaxTextureSlots> Textures;
		uint32_t TextureSoltIndex = 1;													// 0 => WhiteTexture
	};
	static Renderer2DData s_Data;														// ����Ϊջ�Ϸ���


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
		s_Data.QuadVBBase = new QuadVertex[s_Data.MaxVertices];									//����ָ���ʼλ��

		// Generate Samplers
		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		// Shader
		s_Data.TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");				//����glsl������ɫ������
		s_Data.TextureShader->Bind();															//����ɫ������
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);		//�ϴ����в���������Ӧ����Ԫ

		// Texture
		s_Data.WhiteTexture = Texture2D::Create(1,1);											//ͨ��Create�������ÿ�߱ȣ����ݰ�����ɫ���������ڴ棬ֱ�Ӵӵײ㴴����ɫ����
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

		s_Data.QuadIndexCount = 0;																//ÿ������ˢ�£�һ������Ⱦ����Ҫ���ƵĶ���������Ҫ�������¿�ʼ
		s_Data.TextureSoltIndex = 1;															//ÿ������ˢ�£�һ������Ⱦ����Ҫ���Ƶ���������Ҫ��һ���¿�ʼ���ų���ɫ����
		s_Data.QuadVBHind = s_Data.QuadVBBase;													//ÿ������ˢ�£�һ������Ⱦ����Ҫ�����ָ�� Hind ��λ�ø�ֵΪ��ʼλ�ã����¿�ʼ
	}

	void Renderer2D::EndScene()
	{
		NUT_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVBHind - (uint8_t*)s_Data.QuadVBBase;			// Size ���ں��ָ���ȥǰ��(hind �ڻ���ʱһֱ�������ݣ�
		s_Data.QuadVB->SetData(s_Data.QuadVBBase, dataSize);									// Reset VertexBuffer so flush Vertex data ��because of Dynamic Draw)

		Flush();																				// ��������֮����ƣ�ˢ�£�
	}

	void Renderer2D::Flush()
	{
		// Bind texture before rendering
		for (uint32_t i = 0; i < s_Data.TextureSoltIndex; i++)
			s_Data.Textures[i]->Bind(i);															// ������ʹ��"->"���Ƕ����ж�����в�����'.'�Ƕ�������в�����

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

		// ������Ҫ�������߿��ϵ�0,1,2,3������Ž�����ʱ���˳����ã��Ա�õ���ȷ�Ļ��ƽ��
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
		for (uint32_t i = 1; i < s_Data.TextureSoltIndex; i++) {				// ���������鿴���������Ƿ��Ѿ����롣�����У���i�������ʱ����textureIndex�����������������ÿһ��������������Կ��������ţ�ͨ�������е�λ�ñ�ʾ:0,1,2 ...��
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// �������������е�λ����Ϊ����
				break;
			}
		}
		if (textureIndex == 0.0f) {												// ��δ���У���������������У��������µ�s_Data.TextureSoltIndex�������ʱ����textureIndex��������һ��
			s_Data.Textures[s_Data.TextureSoltIndex] = texture;
			textureIndex = float(s_Data.TextureSoltIndex);						// ��TextureSoltIndex = 0��ʼ�������������д����µ��������������һ��.
			
			s_Data.TextureSoltIndex++;
		}

		// ������Ҫ�������߿��ϵ�0,1,2,3������Ž�����ʱ���˳����ã��Ա�õ���ȷ�Ļ��ƽ��
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
		texture->Bind();														//�����������0������Ԫ
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