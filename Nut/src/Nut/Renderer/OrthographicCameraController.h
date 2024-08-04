#pragma once

#include "Nut/Core/Timestep.h"

#include "Nut/Events/MouseEvent.h"
#include "Nut/Events/ApplicationEvent.h"

#include "Nut/Renderer/OrthoGraphicCamera.h"

namespace Nut {

	struct OrthoGraphicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth()  { return Right - Left; }		//正交相机在x轴上视角的宽度
		float GetHeight() { return Top - Bottom; }		//正交相机在y轴上视角的高度
	};

	class OrthoGraphicCameraController
	{
	public:
		OrthoGraphicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthoGraphicCamera& GetCamera() { return m_Camera; }
		const OrthoGraphicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; UpdateViewport(); }
		float GetZoomLevel() const { return m_ZoomLevel; }

		const OrthoGraphicCameraBounds& GetBounds() const { return m_Bounds; }			// 获取摄像机当前位置、朝向（摄像机状态；Left, Right, Bottom, Top）
	private:
		void UpdateViewport();

		bool OnMouseScrolled(MouseScrolledEvent e);
		bool OnWindowResized(WindowResizeEvent e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoGraphicCameraBounds m_Bounds;							// 记录摄像机状态，并用其储存的数据初始化、更新摄像机。摄像机本身没有返回当前数据的函数，故使用Bounds来记录或计算。
		OrthoGraphicCamera m_Camera;								// 初始化 camera 时，我们使用了 bounds 对其赋值，所以必须先声明定义 bounds，否则出现错误

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};

}