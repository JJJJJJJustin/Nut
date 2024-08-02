#include "nutpch.h"

#include "Nut/Renderer/SubTexture2D.h"

namespace Nut
{

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& cellSize /* A single cell size */, const glm::vec2& spritePos, const glm::vec2& spriteSize /* The number of cells which sprite occupies in x/y direction */)
		:m_Texture(texture)
	{
		float sheetWidth = texture->GetWidth(), sheetHeight = texture->GetHeight();
		float spriteWidth = cellSize.x, spriteHeight = cellSize.y;

		glm::vec2 min = {  spritePos.x				   * spriteWidth / sheetWidth,  spritePos.y					* spriteHeight / sheetHeight };			// 精灵图块的左下角(min)
		glm::vec2 max = { (spritePos.x + spriteSize.x) * spriteWidth / sheetWidth, (spritePos.y + spriteSize.y) * spriteHeight / sheetHeight };			// 精灵图块的右上角(max)

		m_TexCoords[0] = { min.x, min.y };			// 获取精灵表中对应精灵图块四个角的坐标
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D>& texture, const glm::vec2& cellSize, const glm::vec2& spritePos, const glm::vec2& spriteSize)
	{
		return CreateRef<SubTexture2D>(texture, cellSize, spritePos, spriteSize);
	}

}