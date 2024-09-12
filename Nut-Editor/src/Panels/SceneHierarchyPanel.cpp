#include "nutpch.h"
#include "SceneHierarchyPanel.h"
#include "Nut/Scene/Component.h"

#include "entt.hpp"

#include <imgui/imgui.h>

namespace Nut
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.view<TagComponent>().each
		(
			[&]() {}
		);

		m_Context->m_Registry.view<entt::entity>().each
		(
			[&]() {}
		);


		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{

	}

}