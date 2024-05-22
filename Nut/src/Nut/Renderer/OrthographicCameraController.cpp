#include "nutpch.h"
#include "OrthographicCameraController.h"

#include "Nut/Core/Input.h"
#include "Nut/Core/KeyCodes.h"

namespace Nut {

	OrthoGraphicCameraController::OrthoGraphicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Rotation(rotation), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
	{ }

	void OrthoGraphicCameraController::OnUpdate(Timestep ts)
	{
		if (m_Rotation) {
			if (Input::IsKeyPressed(NUT_KEY_Q))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			else if (Nut::Input::IsKeyPressed(NUT_KEY_E))
				m_CameraRotation += m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		if (Input::IsKeyPressed(NUT_KEY_A))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_D))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;

		if (Input::IsKeyPressed(NUT_KEY_W))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_S))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
		
	}

	void OrthoGraphicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispathcer(e);
		dispathcer.Dispatch<MouseScrolledEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnMouseScrolled));
		dispathcer.Dispatch<WindowResizeEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnWindowResized));
	}

	bool OrthoGraphicCameraController::OnMouseScrolled(MouseScrolledEvent e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.5f;													//Offset 在向前滚动时通常为负数，向后滚动时通常为正数
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel , m_ZoomLevel);
		return false;																			//这个操作需要一直轮询，故返回 false，不终止该事件的分发操作。
	}

	bool OrthoGraphicCameraController::OnWindowResized(WindowResizeEvent e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();								//设置回调的宽高比
		m_Camera.SetProjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

}
