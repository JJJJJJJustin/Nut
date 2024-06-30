#include "nutpch.h"
#include "OrthoGraphicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nut
{
	OrthoGraphicCamera::OrthoGraphicCamera(float left, float right, float bottom, float top)
		:m_ViewMatrix(1.0f), m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
	{
		NUT_PROFILE_FUNCTION();
 
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoGraphicCamera::SetProjectionMatrix(float left, float right, float bottom, float top)
	{
		NUT_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoGraphicCamera::UpdateViewMatrix()
	{
		NUT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)								//局部空间 -> 世界空间 （乘以model模型矩阵 : glm::translate）
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));		//世界空间 -> 观察空间 （乘以 view观察矩阵 : glm::rotation）

		m_ViewMatrix = glm::inverse(transform);

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}