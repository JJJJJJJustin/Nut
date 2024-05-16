#pragma once

#include <glm/glm.hpp>

namespace Nut
{

	class OrthoGraphicCamera
	{
	public:
		//OrthoGraphicCamera() = default;
		OrthoGraphicCamera(float left, float right, float bottom, float top);

		void SetProjectionMatrix(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position)	{ m_Position = position; UpdateViewMatrix(); }
		void SetRotation(const float& rotation)	    { m_Rotation = rotation; UpdateViewMatrix(); }

		const glm::vec3& GetPosition() const { return m_Position; }
		const float& GetRotation()     const { return m_Rotation; }

		const glm::mat4& GetViewMatrix()			const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix()		const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewProjectionMatrix()	const { return m_ViewProjectionMatrix; }

	private:
		void UpdateViewMatrix();
	private:
		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.0f;

		glm::mat4 m_ViewMatrix;									// 观察矩阵
		glm::mat4 m_ProjectionMatrix;							// 投影矩阵
		glm::mat4 m_ViewProjectionMatrix;						// 上面两个矩阵相乘
	};

}