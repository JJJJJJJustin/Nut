#include "nutpch.h"
#include "SceneHierarchyPanel.h"

#include "Nut/Scene/Component.h"

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

		m_Context->m_Registry.view<entt::entity>().each(
			[&](auto entityID) 
			{
				Entity entity{entityID, m_Context.get()};			// So we can use member function from class Entity
				DrawEntityNode(entity);
			}
		);

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		if(ImGui::IsItemClicked)
			m_SelectionContext = entity;

		// Draw first TreeNode
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());		//??
		
		if(opened)
		{
			// Draw nested TreeNode
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)666999, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();

			ImGui::TreePop();
		}
	}

}