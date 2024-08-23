#pragma once
// Just a simple camera we tend to use when game running

#include <glm/glm.hpp>

namespace Nut
{

	class Camera 
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			:m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};



}