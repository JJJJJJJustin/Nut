#pragma once

#include "glm/glm.hpp"

namespace Nut
{

	struct TransformComponent
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::mat4& transform)
			:Transform(transform) {};
		TransformComponent(const TransformComponent&) = default;

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const glm::vec4& color)
			:Color(color) {};
		SpriteComponent(const SpriteComponent&) = default;

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};

}