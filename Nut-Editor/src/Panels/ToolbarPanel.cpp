#include "nutpch.h"
#include "ToolbarPanel.h"

#include "../EditorLayer.h"

#include <imgui/imgui.h>

namespace Nut
{
	ToolbarPanel::ToolbarPanel()
	{
		m_PlayIcon = Texture2D::Create("E:/VS/Nut/Nut-Editor/Resources/Icons/PlayButton3.png");
		m_StopIcon = Texture2D::Create("E:/VS/Nut/Nut-Editor/Resources/Icons/PauseButton3.png");
	}

	void ToolbarPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##Toolbar panel", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = (m_SceneState == SceneState::Edit ? m_PlayIcon : m_StopIcon);

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void ToolbarPanel::OnScenePlay() 
	{
		m_SceneState = SceneState::Play;
		EditorLayer::Get().m_ActiveScene->OnRuntimeStart();
	}

	void ToolbarPanel::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		EditorLayer::Get().m_ActiveScene->OnRuntimeStart();
	}

}