// 'SceneCamera' Just a subclass of Nut::Camera, which is Specially designed for 'CameraComponent' when game is actuall running. 
// And because of that, we can simply use some member function in "SceneCamera::Camera"( the member in struct "CameraComponent")
#pragma once

#include "Nut/Renderer/Camera.h"

namespace Nut
{
	// 继承 Camera 类中的私有变量 m_ProjectionMatrix,用于该类中。(m_ProjectionMatrix was used in .cpp)
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType{ Perspective = 0, Orthographic = 1 };

		ProjectionType GetProjectionType() { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float orthographicSize, float orthographicNear, float orthographicFar);
		void SetPerspective(float perspectiveVerticalFOV, float perspectiveNear, float perspectiveFar);

		void  SetOrthographicSize(float size) { m_OrthographicSize = size;  UpdateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void  SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; UpdateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void  SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; UpdateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar;  }

		void  SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov;  UpdateProjection(); }		// "FOV" is in radians
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void  SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; UpdateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void  SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; UpdateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		void ViewportResize(uint32_t width, uint32_t height);
	private:
		void UpdateProjection();								// Update camera projection matrix( proj matrix was from parent class: Nut::Camera)
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		// Parameter for orthographic
		float m_AspectRatio = 0.0f;								// Calculated by ViewportResize()

		float m_OrthographicSize = 10.0f;						// Calculate it to init orhtographic Left,Right,Bottom,Top.
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		// Parameter for perspective
		float m_PerspectiveFOV = glm::radians(45.0f);					// ?? Why this number ??
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;	// ?? why this number ?? 
	};


}