// OSAlgorithm
#pragma once

#include "Nut.h"
#include "Panels/SceneHierarchyPanel.h"


namespace Nut {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		// For Getting Instance
		static EditorLayer& Get() { return *s_Instance; }	//ʹ�� * ��ָ�����͵ı��� s_Instance ���н����ã�����ָ�������ָ����������

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	public:
		bool OnKeyPressed(KeyPressedEvent& event);

		glm::vec2 GetImGuiViewportSize() { return m_ViewportSize; };

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		// Instance 
		static EditorLayer* s_Instance;		// s_Instance is a pointer type variable, cuz it takes 'this' pointer and 'nullptr'

		// Scene
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Ref<FrameBuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity, m_RedSquare;
		Entity m_CameraEntity, m_SecondCamera;
		std::vector<Entity> m_Entities;

		OrthoGraphicCameraController m_CameraController;
		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

		//Gizmo
		int m_GizmoType = -1;
	};

}