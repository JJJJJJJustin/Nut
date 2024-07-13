#pragma once

#include "Nut.h"

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Nut::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Nut::Event& e) override;
private:
	Nut::Ref<Nut::Shader> m_Shader;
	Nut::Ref<Nut::Shader> m_SquareShader;
	Nut::ShaderLibrary m_ShaderLibrary;

	Nut::Ref<Nut::VertexArray> m_VertexArray;
	Nut::Ref<Nut::VertexArray> m_SquareVA;

	Nut::Ref<Nut::Texture2D> m_Texture, m_EmojiTexture;

	glm::vec4 m_SquareColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };

	Nut::OrthoGraphicCameraController m_CameraController;

};