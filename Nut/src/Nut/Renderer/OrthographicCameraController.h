#pragma once

#include "Nut/Core/Timestep.h"

#include "Nut/Events/MouseEvent.h"
#include "Nut/Events/ApplicationEvent.h"

#include "Nut/Renderer/OrthoGraphicCamera.h"

namespace Nut {

	class OrthoGraphicCameraController
	{
	public:
		OrthoGraphicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthoGraphicCamera& GetCamera() { return m_Camera; }
		const OrthoGraphicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() const { return m_ZoomLevel; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent e);
		bool OnWindowResized(WindowResizeEvent e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoGraphicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};

}