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
		static EditorLayer& Get(){ return *s_Instance; }	//使用 * 对指针类型的变量 s_Instance 进行解引用，返回指针变量所指向对象的引用

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
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}