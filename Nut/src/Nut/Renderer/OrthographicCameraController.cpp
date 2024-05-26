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

			if (m_CameraRotation > 180.0f)														// ���Ƕȿ����� 180��~ -180����(��360��)���ڲ�Ӱ�������������¼��ټ���ɱ��������ڲ��ϸ����е��½Ƕȹ���
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
		EventDispatcher dispathcer(e);
		dispathcer.Dispatch<MouseScrolledEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnMouseScrolled));
		dispathcer.Dispatch<WindowResizeEvent>(NUT_BIND_EVENT_FN(OrthoGraphicCameraController::OnWindowResized));
	}

	bool OrthoGraphicCameraController::OnMouseScrolled(MouseScrolledEvent e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.5f;													//Offset ����ǰ����ʱͨ��Ϊ������������ʱͨ��Ϊ����
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel , m_ZoomLevel);
		return false;																			//���������Ҫһֱ��ѯ���ʷ��� false������ֹ���¼��ķַ�������
	}

	bool OrthoGraphicCameraController::OnWindowResized(WindowResizeEvent e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();								//���ûص��Ŀ�߱�
		m_Camera.SetProjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

}
