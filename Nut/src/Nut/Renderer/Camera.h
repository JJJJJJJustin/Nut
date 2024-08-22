#pragma once
// Just a simple camera we tend to use when game running

#include <glm/glm.hpp>

namespace Nut
{

	class Camera 
	{
	public:
		Camera(const glm::mat4& projection)
			:m_ProjectionMatrix(projection) {}

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	private:
		glm::mat4 m_ProjectionMatrix;
	};



}