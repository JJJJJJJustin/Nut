#pragma once

#include "Nut/Renderer/Texture.h"

namespace Nut
{
	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};


	class ToolbarPanel
	{
	public:
		ToolbarPanel();

		void OnImGuiRender();

		inline void SetSceneState(SceneState ss) { m_SceneState = ss; }
		inline SceneState GetSceneState() const { return m_SceneState; }
	private:
		void OnScenePlay();
		void OnSceneStop();
		void OnSceneSimulate();
		void OnSceneSimulateStop();


		void ImGuiInfoWindow(const std::string& text);
	private:
		SceneState m_SceneState = SceneState::Edit;

		Ref<Texture2D> m_PlayIcon;
		Ref<Texture2D> m_StopIcon;
		Ref<Texture2D> m_SimulateIcon;

		bool m_ShowPop = false;
	};

}