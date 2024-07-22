#pragma once

#include "Nut.h"

#include "Nut/Renderer/Texture.h"

#include <imgui/imgui.h>


class Sandbox2D : public Nut::Layer 
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Nut::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Nut::Event& event) override;
private:
	Nut::Ref<Nut::Texture2D> m_Texture;
	Nut::Ref<Nut::Texture2D> m_Emoji;

	Nut::OrthoGraphicCameraController m_CameraController;

	glm::vec4 m_SquareColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };
};