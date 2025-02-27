#pragma once

#include "Nut/Renderer/Camera.h"
#include "Nut/Renderer/OrthoGraphicCamera.h"
#include "Nut/Renderer/EditorCamera.h"

#include "Nut/Renderer/Texture.h"
#include "Nut/Renderer/SubTexture2D.h"

#include "Nut/Scene/Component.h"

namespace Nut {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& viewMatrix);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthoGraphicCamera& camera);
		static void EndScene();

		static void StartBatch();
		static void Flush();
		static void FlushAndReset();

		// 图元 Primitive
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, const int& entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const int& entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec2& size, const glm::vec4& color, const int& entityID = -1);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, const int& entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const int& entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const int& entityID = -1);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::mat4& transform, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::mat4& transform, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::mat4& transform, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
	
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, const int& entityID = -1);

		// 精灵 Sprite
		static void DrawQuadSprite(const glm::mat4& transform, const SpriteComponent& src, const int& entityID);
		static void DrawCircleSprite(const glm::mat4& transform, const CircleComponent& src, const int& entityID);

		// Statistics (调试时使用的统计数据，存放在Statistics结构体)
		struct Statistics {
			uint32_t DrawCalls;
			uint32_t GraphicCount;

			// 函数被调用时再计算Vertex或Index，节省性能
			uint32_t GetVertexCount() { return GraphicCount * 4; }
			uint32_t GetIndexCount() { return GraphicCount * 6; }
		};
		static void ClearStats();
		static Statistics GetStats();											// So that we can grab data (in s_Data.Stats) where we want to use 
	};


}