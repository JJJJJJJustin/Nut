#include "nutpch.h"
#include "Nut/Renderer/Renderer2D.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Nut/Renderer/Shader.h"
#include "Nut/Renderer/VertexArray.h"
#include "Nut/Renderer/RendererCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Nut/Renderer/UniformBuffer.h"

namespace Nut {

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Entity only
		int EntityID;
	};

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

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec2 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

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
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVBBase = nullptr;												// 顶点指针起始
		QuadVertex* QuadVBHind = nullptr;												// 顶点指针末尾

		Ref<VertexArray> CircleVA;
		Ref<VertexBuffer> CircleVB;
		Ref<Shader> CircleShader;
		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVBBase = nullptr;
		CircleVertex* CircleVBHind = nullptr;

		float LineWidth = 4;

		Ref<VertexArray> LineVA;
		Ref<VertexBuffer> LineVB;
		Ref<Shader> LineShader;
		uint32_t LineIndexCount = 0;
		LineVertex* LineVBBase = nullptr;
		LineVertex* LineVBHind = nullptr;


		std::array<Ref<Texture2D>, MaxTextureSlots> Textures;
		uint32_t TextureSlotIndex = 1;													// 0 => WhiteTexture

		glm::vec4 QuadVertexPosition[4]{												// 一个方形的基础顶点(默认将其中心放在坐标系原点上，故在使用translate位移时，可直接位移到位移向量的端点。而为什么是glm::vec4组成的元素，是因为需要将transform矩阵左乘给数组中的元素，因为在矩阵的乘法运算中，两个矩阵相乘正确的前提是前一个矩阵列数等于后一个矩阵的行数。
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		glm::vec4 CircleLocalPosition[4]
		{
			{ -1.0f, -1.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f, 0.0f, 1.0f },
			{  1.0f,  1.0f, 0.0f, 1.0f },
			{ -1.0f,  1.0f, 0.0f, 1.0f }
		};

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;
	};
	static Renderer2DData s_Data;														// 更改为栈上分配


	void Renderer2D::Init()
	{
		NUT_PROFILE_FUNCTION();

		//(... for line)
		s_Data.LineVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVB->SetLayout(
			{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"	 },
			{ShaderDataType::Int,    "a_EntityID"}
			}
		);

		s_Data.LineVA = VertexArray::Create();
		s_Data.LineVA->AddVertexBuffer(s_Data.LineVB);
		
		// VertexBuffer + IndexBuffer + VertexArray (... for quad)
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

		// (... for circle)
		s_Data.CircleVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVB->SetLayout(
			{
			{ShaderDataType::Float3, "a_WorldPosition"},
			{ShaderDataType::Float2, "a_LocalPosition"},
			{ShaderDataType::Float4, "a_Color"		  },
			{ShaderDataType::Float,  "a_Thickness"	  },
			{ShaderDataType::Float,  "a_Fade"		  },
			{ShaderDataType::Int,    "a_EntityID"	  }
			}
		);

		s_Data.CircleVA = VertexArray::Create();
		s_Data.CircleVA->SetIndexBuffer(s_Data.QuadIB);				// Tips:We do not create CircleIB for initialize CircleVA cuz we can use QuadIB and we can get the same effect
		s_Data.CircleVA->AddVertexBuffer(s_Data.CircleVB);

		// QuadVertex Ptr
		s_Data.QuadVBBase = new QuadVertex[s_Data.MaxVertices];									//保存指针初始位置
		s_Data.CircleVBBase = new CircleVertex[s_Data.MaxVertices];
		s_Data.LineVBBase = new LineVertex[s_Data.MaxVertices];

		// Shader
		s_Data.QuadShader = Shader::Create("E:/VS/Nut/Nut-Editor/assets/shaders/Renderer2D_Quad.glsl");		//根据glsl创建着色器对象
		s_Data.CircleShader = Shader::Create("E:/VS/Nut/Nut-Editor/assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("E:/VS/Nut/Nut-Editor/assets/shaders/Renderer2D_Line.glsl");

		// UBO
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		// Texture
		s_Data.WhiteTexture = Texture2D::Create(1,1);											//通过Create函数设置宽高比，根据包含颜色数据设置内存，直接从底层创建白色纹理
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Data.Textures[0] = s_Data.WhiteTexture;

	}

	void Renderer2D::Shutdown()
	{
		NUT_PROFILE_FUNCTION();

		delete[] s_Data.QuadVBBase;
		delete[] s_Data.CircleVBBase;
	}

	// If camera belongs to type "Camera", then use this function( ViewMatrix need to be filled in manually )
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& viewMatrix)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(viewMatrix);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthoGraphicCamera& camera)
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera) 
	{
		NUT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		NUT_PROFILE_FUNCTION();

		Flush();																				// 更新数据之后绘制（刷新）
	}

	void Renderer2D::FlushAndReset()															// 如果当前绘制的顶点（或索引）超出了一次批渲染的限制，便开启下一次批渲染，并清零当前记录的顶点数
	{
		EndScene();

		StartBatch();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)																// 如果依次运行下来后发现没有需要绘制的图形，则返回空并跳出，不运行之后代码以节省性能
		{
			uint32_t dataSize = uint32_t((uint8_t*)s_Data.QuadVBHind - (uint8_t*)s_Data.QuadVBBase);// Size 等于后端指针减去前端(hind 在绘制时一直更新数据）
			s_Data.QuadVB->SetData(s_Data.QuadVBBase, dataSize);									// Reset VertexBuffer so flush Vertex data （because of Dynamic Draw)

			// Bind texture before rendering
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.Textures[i]->Bind(i);														// 对数组使用"->"才是对其中对象进行操作，'.'是对数组进行操作。

			s_Data.QuadShader->Bind();
			RendererCommand::DrawIndexed(s_Data.QuadVA, s_Data.QuadIndexCount);						// 运行至此处时进行渲染
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = uint32_t((uint8_t*)s_Data.CircleVBHind - (uint8_t*)s_Data.CircleVBBase);
			s_Data.CircleVB->SetData(s_Data.CircleVBBase, dataSize);

			s_Data.CircleShader->Bind();
			RendererCommand::DrawIndexed(s_Data.CircleVA, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineIndexCount)
		{
			uint32_t dataSize = uint32_t((uint8_t*)s_Data.LineVBHind - (uint8_t*)s_Data.LineVBBase);
			s_Data.LineVB->SetData(s_Data.LineVBBase, dataSize);

			s_Data.LineShader->Bind();
			RendererCommand::SetLineWidth(s_Data.LineWidth);
			RendererCommand::DrawLines(s_Data.LineVA, s_Data.LineIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::StartBatch() 
	{
		s_Data.QuadIndexCount = 0;																//每结束一次场景（依次场景中可能包含多个批渲染调用），需要绘制的顶点索引数要从零重新开始
		s_Data.TextureSlotIndex = 1;															//每结束一次场景（依次场景中可能包含多个批渲染调用），需要绘制的纹理索引要从一重新开始（排除白色纹理）
		s_Data.QuadVBHind = s_Data.QuadVBBase;													//每结束一次场景（依次场景中可能包含多个批渲染调用），需要将后端指针 Hind 的位置赋值为起始位置，从新开始

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVBHind = s_Data.CircleVBBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVBHind = s_Data.LineVBBase;
	}

	// -------------------------- Draw func ------------------------------------------------------------------
	// -------------------------- Draw Quad
	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, const int& entityID)
	{
		NUT_PROFILE_FUNCTION();

		/*if (s_Data.LineIndexCount >= s_Data.MaxIndices)
		{
			FlushAndReset();
		}*/
		s_Data.LineVBHind->Position = p0;
		s_Data.LineVBHind->Color = color;
		s_Data.LineVBHind->EntityID = entityID;
		s_Data.LineVBHind++;

		s_Data.LineVBHind->Position = p1;
		s_Data.LineVBHind->Color = color;
		s_Data.LineVBHind->EntityID = entityID;
		s_Data.LineVBHind++;

		s_Data.LineIndexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const int& entityID)
	{
		glm::vec3 p0 = { position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z };
		glm::vec3 p1 = { position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z };
		glm::vec3 p2 = { position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z };
		glm::vec3 p3 = { position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z };

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, const int& entityID)
	{
		glm::vec3 verticesPos[4];
		for(int i = 0; i < 4; i++)
			verticesPos[i] = transform * s_Data.QuadVertexPosition[i];

		DrawLine(verticesPos[0], verticesPos[1], color, entityID);
		DrawLine(verticesPos[1], verticesPos[2], color, entityID);
		DrawLine(verticesPos[2], verticesPos[3], color, entityID);
		DrawLine(verticesPos[3], verticesPos[0], color, entityID);
	}

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

																				// 顶点需要被按照线框上的0,1,2,3顶点序号进行逆时针的顺序放置，以便得到正确的绘制结果
																				// !!!不要写成 s_Data.QuadVertexPosition[0] * transform，给 transform 左乘一个位置向量这样的操作是无效的。
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

		s_Data.Stats.GraphicCount++;
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
	#pragma region 在纹理集中搜寻纹理
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {				// 遍历纹理，查看现有纹理是否已经存入。若命中，则将i赋予给临时变量textureIndex，并跳出。（这里的每一个纹理的索引可以看做是其编号，通过纹理集中的位置表示:0,1,2 ...）
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// 将纹理在纹理集中的位置作为索引
				break;
			}
		}
		if (textureIndex == 0.0f) {												// 若未命中，则将纹理放入纹理集中，并将最新的s_Data.TextureSlotIndex赋予给临时变量textureIndex，并自增一次
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {			// 如果纹理数也多于最大值，就刷新一次批渲染
				FlushAndReset();
			}
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			textureIndex = float(s_Data.TextureSlotIndex);						// 从TextureSlotIndex = 0开始，依次相纹理集中存入新的纹理。存入后自增一次.

			s_Data.TextureSlotIndex++;
		}
	#pragma endregion

		// 顶点需要被按照线框上的0,1,2,3顶点序号进行逆时针的顺序放置，以便得到正确的绘制结果
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

		s_Data.Stats.GraphicCount++;
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
	#pragma region 在纹理集中搜寻纹理
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {				// 遍历纹理，查看现有纹理是否已经存入。若命中，则将i赋予给临时变量textureIndex，并跳出。（这里的每一个纹理的索引可以看做是其编号，通过纹理集中的位置表示:0,1,2 ...）
			if (*s_Data.Textures[i].get() == *texture.get()) {
				textureIndex = (float)i;										// 将纹理在纹理集中的位置作为索引
				break;
			}
		}
		if (textureIndex == 0.0f) {												// 若未命中，则将纹理放入纹理集中，并将最新的s_Data.TextureSlotIndex赋予给临时变量textureIndex，并自增一次
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots) {			// 如果纹理数也多于最大值，就刷新一次批渲染
				FlushAndReset();
			}
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			textureIndex = float(s_Data.TextureSlotIndex);						// 从TextureSlotIndex = 0开始，依次相纹理集中存入新的纹理。存入后自增一次.

			s_Data.TextureSlotIndex++;
		}
	#pragma endregion

		// 顶点需要被按照线框上的0,1,2,3顶点序号进行逆时针的顺序放置，以便得到正确的绘制结果
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

		s_Data.Stats.GraphicCount++;
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

		s_Data.Stats.GraphicCount++;
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
	#pragma region 在纹理集中搜寻纹理
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

		s_Data.Stats.GraphicCount++;
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
	#pragma region 在纹理集中搜寻纹理
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

		s_Data.Stats.GraphicCount++;
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

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*=1.0f*/, float fade /*=0.005f*/, const int& entityID /*= -1*/)
	{
		NUT_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount >= s_Data.MaxIndices) 
		{
			FlushAndReset();
		}

		constexpr size_t circleVertexCount = 4;

		for (size_t i = 0; i < circleVertexCount; i++) 
		{
			s_Data.CircleVBHind->WorldPosition = transform * s_Data.QuadVertexPosition[i];
			s_Data.CircleVBHind->LocalPosition = s_Data.CircleLocalPosition[i];
			s_Data.CircleVBHind->Color = color;
			s_Data.CircleVBHind->Thickness = thickness;
			s_Data.CircleVBHind->Fade = fade;
			s_Data.CircleVBHind->EntityID = entityID;
			s_Data.CircleVBHind++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.Stats.GraphicCount++;
	}

	// ------------------------------------- Draw Sprite -------------------------------------------------
	void Renderer2D::DrawQuadSprite(const glm::mat4& transform, const SpriteComponent& src, const int& entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::DrawCircleSprite(const glm::mat4& transform, const CircleComponent& src, const int& entityID)
	{
		DrawCircle(transform, src.Color, src.Thickness, src.Fade, entityID);
	}



	void Renderer2D::ClearStats()
	{
		//s_Data.Stats.DrawCalls = 0;
		//s_Data.Stats.GraphicCount = 0;
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}



}