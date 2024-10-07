#include "nutpch.h"
#include "SceneHierarchyPanel.h"

#include "Nut/Scene/Component.h"
#include "../EditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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

		// Right-click on blank space of the window
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))	// Disable 'ContextWindow' from covering previous 'ContextItem Menu'(Which was in SceneHierarchyPanel::DrawEntityNode)
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);
		ImGui::End();
	}



	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		// Draw first TreeNode
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if(ImGui::IsItemClicked())				// Needs to be put under the ImGuiTreeNodeEx
			m_SelectionContext = entity;
		
		// If you do not mark this temporary variable "deleted" as true(After this code: ImGui::MenuItem), 
		// but delete the entity directly in the ContextItem Menu, 
		// the program will crash because it is still using the previously deleted entity 
		// (in the conditional judgment of the second tree node rendering)
		
		bool entityDeleted = false;
		if(ImGui::BeginPopupContextItem())
		{
			// Define this menu according to your needs
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if(opened)
		{
			// Draw nested TreeNode
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)666999, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();

			ImGui::TreePop();
		}

		if (entityDeleted) {
			m_Context->DestroyEntity(entity);
			if(entity == m_SelectionContext)
				m_SelectionContext = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity& entity)
	{
		// For Tag component, we not use template function: SceneHierarchyPanel::DrawComponent, because it cannot be deleted.
		if(entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			// Clear old data and then fill with new data
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());		// It's Safer than strcpy() :)
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				// Update tag, so next time when we copy string from tag, the buffer data will be init by latest string
				tag = std::string(buffer);
			};

			ImGui::SameLine();
			ImGui::PushItemWidth(-1); {
				// Draw "add component menu"
				if (ImGui::Button("AddComponent"))
					ImGui::OpenPopup("AddComponentMenu");
				if (ImGui::BeginPopup("AddComponentMenu"))
				{
					if (ImGui::MenuItem("CameraComponent"))
					{
						m_SelectionContext.AddComponent<CameraComponent>();
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::MenuItem("SpriteRendererComponent"))
					{
						m_SelectionContext.AddComponent<SpriteComponent>();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			ImGui::PopItemWidth();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Controller("Translation", component.Translation);

			glm::vec3 rotation = glm::degrees(component.Rotation);	// Check the notes to learn more
			DrawVec3Controller("Rotation", rotation);
			component.Rotation = glm::radians(rotation);

			DrawVec3Controller("Scale", component.Scale, 1.0f);	// Scale will be reset but at least 1.0f
		});


		// I passed 'This pointer' though lambda expression (because I use Non-static member variables: m_Context,I need to specify the capture method)
		DrawComponent<CameraComponent>("Camera", entity, [this](auto& component)
		{
			auto& camera = component.Camera;
			bool& primary = component.Primary;
			bool& fixedAspectRatio = component.FixedAspectRatio;

			//  -------- Draw Check Box --------
			ImGui::Checkbox("Primary", &primary);
			// --------- Draw Combo Box --------
			const char* projectionType[] = { "Perspective", "Orthographic" };
			const char* currentProjectionType = projectionType[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionType))			// Combo box preview value needs to be a c_str
			{
				// -------- Draw drop-down Selection List --------
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = (projectionType[i] == currentProjectionType);
					if (ImGui::Selectable(projectionType[i], isSelected))		// (What isSelected do:) Is this option is current projection type ? Default highlight : Not highlight
					{
						currentProjectionType = projectionType[i];				// If you select one projection, then update current projection type string as latest
						camera.SetProjectionType((SceneCamera::ProjectionType)i);

						glm::vec2 viewportSize = EditorLayer::Get().GetImGuiViewportSize();
						m_Context->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();	// 用于更新焦点（焦点不同于高亮显示）
				}
				ImGui::EndCombo();
			}

			// -------- Draw Perspective Camera Controller --------
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &verticalFov, 1.0f, 30.0f, 120.0f))
					camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			// -------- Draw Orthographic Camera Controller --------
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspectRatio);

				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);
			}
		});

		DrawComponent<SpriteComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
		});
	}

	// ------------------------ Some definitions ------------------------------------------
	template<typename T, typename UIFunction>
	static void SceneHierarchyPanel::DrawComponent(const std::string& name, Entity& entity, UIFunction uiFunc)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();									// We need to get the correct size of the available content region, which is in the properties window, not in the tree node drawn in this window

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			float lineHeight = ImGui::CalcTextSize("+").y + GImGui->Style.FramePadding.y * 2.0f;		// We need to get the correct size of the button, which is set in the StyleVar scope
			// Draw tree node
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			// Draw component settings menu (including remove button)
			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
			if (ImGui::Button("+", { lineHeight, lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunc(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue /*= 0.0f*/, float columnWidth /*= 100.0f*/)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());

		ImGui::NextColumn();
		{
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());		// PopItemWidth as many as you set (Why components is 3?and which 3 components it control? what the specification of determine which component you want to manage?
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
			// --------- x -----------
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.6f, 0.1f, 0.0f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.1f, 0.0f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6f, 0.1f, 0.0f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);									// PopStyleColor as many as you set

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();										// PopItemWidth()
			// ---------- y -----------
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.6f, 0.1f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.9f, 0.1f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.6f, 0.1f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			// ----------- z ------------
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.0f, 0.6f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.0f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.0f, 0.6f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
		}

		ImGui::Columns(1);		// Rostore column status as 1 column

		ImGui::PopID();
	}



}