#include "nutpch.h"
#include "Nut/Renderer/Renderer2D.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Nut/Renderer/Shader.h"
#include "Nut/Renderer/VertexArray.h"
#include "Nut/Renderer/RendererCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Nut/Renderer/UniformBuffer.h"

namespace Nut {

	struct QuadVertex 
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Entity only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 1000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices  = MaxQuads * 6;
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
		uint32_t TextureSlotIndex = 1;													// 0 => WhiteTexture

		glm::vec4 QuadVertexPosition[4]{												// һ�����εĻ�������(Ĭ�Ͻ������ķ�������ϵԭ���ϣ�����ʹ��translateλ��ʱ����ֱ��λ�Ƶ�λ�������Ķ˵㡣��Ϊʲô��glm::vec4��ɵ�Ԫ�أ�����Ϊ��Ҫ��transform������˸������е�Ԫ�أ���Ϊ�ھ���ĳ˷������У��������������ȷ��ǰ����ǰһ�������������ں�һ�������������
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;
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
			{ShaderDataType::Float, "a_TilingFactor"},
			{ShaderDataType::Int, "a_EntityID"}
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

		// Shader
		s_Data.TextureShader = Shader::Create("E:/VS/Nut/Nut-Editor/assets/shaders/TextureShader.glsl");		//����glsl������ɫ������
		
		// UBO
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		// Texture
		s_Data.WhiteTexture = Texture2D::Create(1,1);											//ͨ��Create�������ÿ�߱ȣ����ݰ�����ɫ���������ڴ棬ֱ�Ӵӵײ㴴����ɫ����
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Data.Textures[0] = s_Data.WhiteTexture;

	}

	void Renderer2D::Shutdown()
	{
		NUT_PROFILE_FUNCTION();

		delete[] s_Data.QuadVBBase;
	}

	// If camera belongs to type "Camera", then use this function( ViewMatrix need to be filled in manually )
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& viewMatrix)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(viewMatrix);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVBHind = s_Data.QuadVBBase;
	}

	void Renderer2D::BeginScene(const OrthoGraphicCamera& camera)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		s_Data.QuadIndexCount = 0;																//ÿ����һ�γ��������γ����п��ܰ����������Ⱦ���ã�����Ҫ���ƵĶ���������Ҫ�������¿�ʼ
		s_Data.TextureSlotIndex = 1;															//ÿ����һ�γ��������γ����п��ܰ����������Ⱦ���ã�����Ҫ���Ƶ���������Ҫ��һ���¿�ʼ���ų���ɫ����
		s_Data.QuadVBHind = s_Data.QuadVBBase;													//ÿ����һ�γ��������γ����п��ܰ����������Ⱦ���ã�����Ҫ�����ָ�� Hind ��λ�ø�ֵΪ��ʼλ�ã����¿�ʼ
	}

	void Renderer2D::BeginScene(const EditorCamera& camera) 
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVBHind = s_Data.QuadVBBase;
	}

	void Renderer2D::EndScene()
	{
		NUT_PROFILE_FUNCTION();

		uint32_t dataSize = uint32_t((uint8_t*)s_Data.QuadVBHind - (uint8_t*)s_Data.QuadVBBase);// Size ���ں��ָ���ȥǰ��(hind �ڻ���ʱһֱ�������ݣ�
		s_Data.QuadVB->SetData(s_Data.QuadVBBase, dataSize);									// Reset VertexBuffer so flush Vertex data ��because of Dynamic Draw)

		Flush();																				// ��������֮����ƣ�ˢ�£�
	}

	void Renderer2D::FlushAndReset()															// �����ǰ���ƵĶ��㣨��������������һ������Ⱦ�����ƣ��㿪����һ������Ⱦ�������㵱ǰ��¼�Ķ�����
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVBHind = s_Data.QuadVBBase;
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount == 0)
			return;																				// �������������������û����Ҫ���Ƶ�ͼ�Σ��򷵻ؿղ�������������֮������Խ�ʡ����

		// Bind texture before rendering
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.Textures[i]->Bind(i);														// ������ʹ��"->"���Ƕ����ж�����в�����'.'�Ƕ�������в�����

		s_Data.TextureShader->Bind();
		RendererCommand::DrawIndexed(s_Data.QuadVA, s_Data.QuadIndexCount);						// �������˴�ʱ������Ⱦ
		s_Data.Stats.DrawCalls++;
	}

	// -------------------------- Draw func ------------------------------------------------------------------
	// -------------------------- Draw Quad
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const int& entityID)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {						// If Indices more than batch rendering can include,then start new batch rendering
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		constexpr float textureIndex = 0.0f;									// Just use the white texutre
		constexpr float tilingFactor = 1.0f;									// Single color don't need tiling factor

																				// ������Ҫ�������߿��ϵ�0,1,2,3������Ž�����ʱ���˳����ã��Ա�õ���ȷ�Ļ��ƽ��
																				// !!!��Ҫд�� s_Data.QuadVertexPosition[0] * transform���� transform ���һ��λ�����������Ĳ�������Ч�ġ�
		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = color;
			s_Data.QuadVBHind->TexCoord = texCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind->EntityID = entityID;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 0.0f });

		DrawQuad(transform, color);							// calc the transform then draw it with "matrix function"
	}

	// -------------------------- Draw Texture
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const int& entityID)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		float textureIndex = 0.0f;
	#pragma region ����������Ѱ����
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {				// ���������鿴���������Ƿ��Ѿ����롣�����У���i�������ʱ����textureIndex�����������������ÿһ��������������Կ��������ţ�ͨ�������е�λ�ñ�ʾ:0,1,2 ...��
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// �������������е�λ����Ϊ����
				break;
			}
		}
		if (textureIndex == 0.0f) {												// ��δ���У���������������У��������µ�s_Data.TextureSlotIndex�������ʱ����textureIndex��������һ��
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {			// ���������Ҳ�������ֵ����ˢ��һ������Ⱦ
				FlushAndReset();
			}
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			textureIndex = float(s_Data.TextureSlotIndex);						// ��TextureSlotIndex = 0��ʼ�������������д����µ��������������һ��.

			s_Data.TextureSlotIndex++;
		}
	#pragma endregion

		// ������Ҫ�������߿��ϵ�0,1,2,3������Ž�����ʱ���˳����ã��Ա�õ���ȷ�Ļ��ƽ��
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = tintColor;
			s_Data.QuadVBHind->TexCoord = texCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind->EntityID = entityID;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 0.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	// -------------------------- Draw SubTexture
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor, const int& entityID)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* subTexCoords = subtexture->GetCoords();
		Ref<Texture2D>texture = subtexture->GetTexture();

		float textureIndex = 0.0f;
	#pragma region ����������Ѱ����
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {				// ���������鿴���������Ƿ��Ѿ����롣�����У���i�������ʱ����textureIndex�����������������ÿһ��������������Կ��������ţ�ͨ�������е�λ�ñ�ʾ:0,1,2 ...��
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// �������������е�λ����Ϊ����
				break;
			}
		}
		if (textureIndex == 0.0f) {												// ��δ���У���������������У��������µ�s_Data.TextureSlotIndex�������ʱ����textureIndex��������һ��
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {			// ���������Ҳ�������ֵ����ˢ��һ������Ⱦ
				FlushAndReset();
			}
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			textureIndex = float(s_Data.TextureSlotIndex);						// ��TextureSlotIndex = 0��ʼ�������������д����µ��������������һ��.

			s_Data.TextureSlotIndex++;
		}
	#pragma endregion

		// ������Ҫ�������߿��ϵ�0,1,2,3������Ž�����ʱ���˳����ã��Ա�õ���ȷ�Ļ��ƽ��
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = tintColor;
			s_Data.QuadVBHind->TexCoord = subTexCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind->EntityID = entityID;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		NUT_PROFILE_FUNCTION();
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 0.0f });

		DrawQuad(transform, subtexture, tilingFactor, tintColor);
	}

	// ------------------------------------- Rotated Quad --------------------------------------------------------------
	// ------------------------------------- Draw Quad
	void Renderer2D::DrawRotatedQuad(const glm::mat4& transform, float rotation, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = color;
			s_Data.QuadVBHind->TexCoord = texCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawRotatedQuad(transform, rotation, color);
	}

	// ------------------------------------- Draw Texture
	void Renderer2D::DrawRotatedQuad(const glm::mat4& transform, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;
	#pragma region ����������Ѱ����
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {
				FlushAndReset();
			}
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			textureIndex = float(s_Data.TextureSlotIndex);

			s_Data.TextureSlotIndex++;
		}
	#pragma endregion

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = tintColor;
			s_Data.QuadVBHind->TexCoord = texCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawRotatedQuad(transform, rotation, texture, tilingFactor, tintColor);
	}

	// ------------------------------------- Draw SubTexture
	void Renderer2D::DrawRotatedQuad(const glm::mat4& transform, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
			FlushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* subTexCoords = subtexture->GetCoords();
		Ref<Texture2D> texture = subtexture->GetTexture();

		float textureIndex = 0.0f;
	#pragma region ����������Ѱ����
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
				if (*s_Data.Textures[i].get() == *texture.get()) {
					textureIndex = (float)i;
					break;
				}
			}
			if (textureIndex == 0.0f) {
				if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {
					FlushAndReset();
				}
				s_Data.Textures[s_Data.TextureSlotIndex] = texture;
				textureIndex = float(s_Data.TextureSlotIndex);

				s_Data.TextureSlotIndex++;
			}
	#pragma endregion

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVBHind->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVBHind->Color = tintColor;
			s_Data.QuadVBHind->TexCoord = subTexCoords[i];
			s_Data.QuadVBHind->TexIndex = textureIndex;
			s_Data.QuadVBHind->TilingFactor = tilingFactor;
			s_Data.QuadVBHind++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawRotatedQuad(transform, rotation, subtexture, tilingFactor, tintColor);
	}



	void Renderer2D::ClearStats()
	{
		//s_Data.Stats.DrawCalls = 0;
		//s_Data.Stats.QuadCount = 0;
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}



}