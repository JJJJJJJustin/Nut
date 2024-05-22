#pragma once

#include "Nut.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	Nut::Ref<Nut::Shader> m_SquareShader;
	Nut::Ref<Nut::VertexArray> m_SquareVA;
	Nut::OrthoGraphicCameraController m_CameraController;

	glm::vec4 m_SquareColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };
};