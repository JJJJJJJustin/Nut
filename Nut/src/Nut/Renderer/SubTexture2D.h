#pragma once

#include <glm/glm.hpp>

#include "Nut/Renderer/Texture.h"

namespace Nut
{

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& cellSize, const glm::vec2& spritePos, const glm::vec2& spriteSize);

		static Ref<SubTexture2D> Create(const Ref<Texture2D>& texture, const glm::vec2& cellSize, const glm::vec2& spritePos, const glm::vec2& spriteSize = { 1, 1 });	// spriteSize д╛хон╙{1,1}

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetCoords() const { return m_TexCoords; }
	private:
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};

}