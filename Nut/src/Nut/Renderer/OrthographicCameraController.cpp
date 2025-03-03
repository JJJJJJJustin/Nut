#include "nutpch.h"
#include "Nut/Renderer/OrthographicCameraController.h"

#include "Nut/Core/Input.h"
#include "Nut/Core/KeyCodes.h"

namespace Nut {

	OrthoGraphicCameraController::OrthoGraphicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Rotation(rotation), m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top),
		 m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel })
	{ }

	void OrthoGraphicCameraController::OnUpdate(Timestep ts)
	{
		NUT_PROFILE_FUNCTION();
		
		if (m_Rotation) {
			if (Input::IsKeyPressed(NUT_KEY_Q))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			else if (Nut::Input::IsKeyPressed(NUT_KEY_E))
				m_CameraRotation += m_CameraRotationSpeed * ts;

			if (m_CameraRotation > 180.0f)														// 将角度控制在 180°~ -180°中(共360°)，在不影响计算结果的情况下减少计算成本（避免在不断更新中导致角度过大）
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.SetRotation(m_CameraRotation);
		}

		if (Input::IsKeyPressed(NUT_KEY_A)) {
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		} else if (Nut::Input::IsKeyPressed(NUT_KEY_D)) {
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(NUT_KEY_W)) {
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -=  cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		} else if (Nut::Input::IsKeyPressed(NUT_KEY_S)) {
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y +=  cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
		
	}

	void OrthoGraphicCameraController::OnEvent(Event& e)
	{
		NUT_PROFILE_FUNCTION();
		
		EventDispatcher dispathcer(e);
		dispathcer.Dispatch<MouseScrolledEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnMouseScrolled));
		dispathcer.Dispatch<WindowResizeEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnWindowResized));
	}

	void OrthoGraphicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		UpdateViewport();
	}
	
	void OrthoGraphicCameraController::UpdateViewport()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };	//更新物体所在的世界空间的边界，以便更新矩阵
		m_Camera.SetProjectionMatrix(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthoGraphicCameraController::OnMouseScrolled(MouseScrolledEvent e)
	{
		NUT_PROFILE_FUNCTION();
		
		m_ZoomLevel -= e.GetYOffset() * 0.5f;													//Offset 在向前滚动时通常为负数，向后滚动时通常为正数
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		UpdateViewport();
		return false;																			//这个操作需要一直轮询，故返回 false，不终止该事件的分发操作。
	}

	bool OrthoGraphicCameraController::OnWindowResized(WindowResizeEvent e)
	{
		NUT_PROFILE_FUNCTION();
		
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();								//设置回调的宽高比
		UpdateViewport();
		return false;
	}

}
