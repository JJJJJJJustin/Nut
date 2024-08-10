#pragma once

#include "Nut.h"

#include "ParticleSystem.h"


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
	Nut::Ref<Nut::Texture2D> m_Texture, m_Emoji;
	Nut::Ref<Nut::Texture2D> m_SpriteSheet;

	std::unordered_map<char, Nut::Ref<Nut::SubTexture2D>> m_TilesMap;

	Nut::OrthoGraphicCameraController m_CameraController;

	glm::vec4 m_QuadColor= { 0.5412f, 0.1686f, 0.8863f, 1.0f };

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};