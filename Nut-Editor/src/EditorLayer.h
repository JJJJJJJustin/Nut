#pragma once

#include "Nut.h"
#include "Panels/SceneHierarchyPanel.h"


namespace Nut {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	private:
		// Scene
		Ref<Texture2D> m_Texture, m_Emoji;

		Ref<FrameBuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity, m_RedSquare;
		Entity m_CameraEntity, m_SecondCamera;

		bool m_PrimaryCamera = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		OrthoGraphicCameraController m_CameraController;

		glm::vec4 m_QuadColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };

		// Panels
		SceneHierarchyPanel m_HierarchyPanel;
	};

}