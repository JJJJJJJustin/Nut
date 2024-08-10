#pragma once

#include "Nut.h"


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
		Ref<Texture2D> m_Texture, m_Emoji;
		Ref<Texture2D> m_SpriteSheet, m_SpriteSheetRole;
		Ref<SubTexture2D> m_Role;

		std::unordered_map<char, Ref<SubTexture2D>> m_TilesMap;

		Ref<FrameBuffer> m_Framebuffer;

		OrthoGraphicCameraController m_CameraController;

		glm::vec4 m_QuadColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };

	};

}