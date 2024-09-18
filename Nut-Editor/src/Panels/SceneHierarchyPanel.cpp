#include "nutpch.h"
#include "SceneHierarchyPanel.h"

#include "Nut/Scene/Component.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())		// When mouse left was pressed and mouse position was in SceneHierarchy Panel, then...
			m_SelectionContext = {};

		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
			DrawComponents(m_SelectionContext);
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		// Draw first TreeNode
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if(ImGui::IsItemClicked())				// Needs to be put under the ImGuiTreeNodeEx
			m_SelectionContext = entity;
		
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

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if(entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			
			// Clear old data and then fill with new data
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));	
			strcpy_s(buffer, sizeof(buffer), tag.c_str());		// It's Safer than strcpy() :)
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) 
			{
				// Update tag, so next time when we copy string from tag, the buffer data will be init by latest string
				tag = std::string(buffer);
			};
		}

		if (entity.HasComponent<TransformComponent>())
		{
			if(ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
			}
			ImGui::TreePop();
		}


	}






}