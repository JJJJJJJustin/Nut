#pragma once

#include "entt.hpp"

#include "Nut/Scene/Scene.h"

namespace Nut
{

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, Scene* scene);
		Entity(const Entity& other) = default;
		//Entity& operator=(const Entity& other) = default;

		~Entity() = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			NUT_CORE_ASSERT(!HasComponent<T>(), "This Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			NUT_CORE_ASSERT(HasComponent<T>(), "This Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			NUT_CORE_ASSERT(HasComponent<T>(), "This Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }		// We use it in: SceneHierarchyPanel::DrawEntityNode() -> ImGui::TreeNodeEx()
		operator entt::entity() const { return m_EntityHandle; }			// We use it in: Scene::DestroyEntity()
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }	// We use it in: SceneHeirarchyPanel::DrawEntityNode() -> ImGuiTreeNodeFlags flags = ...
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
	


}