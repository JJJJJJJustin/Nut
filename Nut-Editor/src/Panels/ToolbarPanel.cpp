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
		m_SimulateIcon = Texture2D::Create("E:/VS/Nut/Nut-Editor/Resources/Icons/SimulateButton.png");
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

		{
			Ref<Texture2D> icon = ((m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_PlayIcon : m_StopIcon);
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		ImGui::SameLine();
		{
			Ref<Texture2D> icon2 = ((m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_SimulateIcon : m_StopIcon);

			if (ImGui::ImageButton((ImTextureID)icon2->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneSimulateStop();
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();

		if (m_ShowPop)
		{
			ImGui::OpenPopup("Info Popup");
			ImGuiInfoWindow("Please load scene first!\nOr you can create new scene first");
		}
	}

	void ToolbarPanel::OnScenePlay() 
	{
		if(m_SceneState == SceneState::Simulate)
		{
			OnSceneStop();
		}

		// Get scene & panel data (Which is References types) from EditorLayer
		Ref<Scene>& activeScene = EditorLayer::Get().m_ActiveScene;
		Ref<Scene>& editorScene = EditorLayer::Get().m_EditorScene;
		SceneHierarchyPanel& sceneHierarchyPanel = EditorLayer::Get().m_SceneHierarchyPanel;
		if (editorScene != nullptr) 
		{
			m_SceneState = SceneState::Play;
			activeScene = Scene::Copy(editorScene);					// When playing, active scene is the duplicate of editor scene

			activeScene->OnRuntimeStart();
			sceneHierarchyPanel.SetContext(activeScene);
		}
		else
		{
			NUT_CORE_CRITICAL("There is no active scene to used(should load scene first)! Or you can create new scene first");
			m_ShowPop = true;
		}
	}

	void ToolbarPanel::OnSceneStop()
	{
		Ref<Scene>& activeScene = EditorLayer::Get().m_ActiveScene;
		Ref<Scene>& editorScene = EditorLayer::Get().m_EditorScene;
		SceneHierarchyPanel& sceneHierarchyPanel = EditorLayer::Get().m_SceneHierarchyPanel;

		m_SceneState = SceneState::Edit;
		activeScene = editorScene;								// When editing, active scene is the editor scene

		activeScene->OnRuntimeStop();
		sceneHierarchyPanel.SetContext(activeScene);

	}

	void ToolbarPanel::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
		{
			OnSceneSimulateStop();
		}

		// Get scene & panel data (Which is References types) from EditorLayer
		Ref<Scene>& activeScene = EditorLayer::Get().m_ActiveScene;
		Ref<Scene>& editorScene = EditorLayer::Get().m_EditorScene;
		SceneHierarchyPanel& sceneHierarchyPanel = EditorLayer::Get().m_SceneHierarchyPanel;
		if (editorScene != nullptr)
		{
			m_SceneState = SceneState::Simulate;
			activeScene = Scene::Copy(editorScene);					// When playing, active scene is the duplicate of editor scene

			activeScene->OnSimulationStart();
			sceneHierarchyPanel.SetContext(activeScene);
		}
		else
		{
			NUT_CORE_CRITICAL("There is no active scene to used(should load scene first)! Or you can create new scene first");
			m_ShowPop = true;
		}
	}

	void ToolbarPanel::OnSceneSimulateStop()
	{
		Ref<Scene>& activeScene = EditorLayer::Get().m_ActiveScene;
		Ref<Scene>& editorScene = EditorLayer::Get().m_EditorScene;
		SceneHierarchyPanel& sceneHierarchyPanel = EditorLayer::Get().m_SceneHierarchyPanel;

		m_SceneState = SceneState::Edit;
		activeScene = editorScene;								// When editing, active scene is the editor scene

		activeScene->OnSimulationStop();
		sceneHierarchyPanel.SetContext(activeScene);
	}

	void ToolbarPanel::ImGuiInfoWindow(const std::string& text)
	{
		if (ImGui::BeginPopupModal("Info Popup", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text(text.c_str());
			ImGui::Separator();

			// 设置光标位置使按钮居中
			float buttonWidth = ImGui::CalcTextSize("OK").x + ImGui::GetStyle().FramePadding.x * 2.0f;
			float windowWidth = ImGui::GetWindowSize().x;
			float centerX = (windowWidth - buttonWidth) * 0.5f;

			ImGui::SetCursorPosX(centerX);

			// 添加一个确认按钮
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
				m_ShowPop = false;
			}

			ImGui::EndPopup();
		}
	}

}