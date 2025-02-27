#pragma once

#include "Nut/Renderer/RendererAPI.h"

namespace Nut {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void Clear() override;
		void SetClearColor(const glm::vec4& color) override;

		void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
		void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
	};

}