// 'SceneCamera' Just a subclass of Nut::Camera, which is Specially designed for 'CameraComponent' when game is actuall running. 
// And because of that, we can simply use some member function in "SceneCamera::Camera"( the member in struct "CameraComponent")
#pragma once

#include "Nut/Renderer/Camera.h"

namespace Nut
{
	// 继承 Camera 类中的私有变量 m_ProjectionMatrix,用于该类中。
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float orthographicSize, float orthographicNear, float orthographicFar);

		void SetOrthographicSize(float size) { m_OrthographicSize = size;  UpdateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }

		void ViewportResize(uint32_t width, uint32_t height);
	private:
		void UpdateProjection();								// Update camera projection matrix( proj matrix was from parent class: Nut::Camera)
	private:
		float m_AspectRatio = 0.0f;

		float m_OrthographicSize = 10.0f;						// Calculate it to init orhtographic Left,Right,Bottom,Top.
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
	};


}