#pragma once

#include <glm/glm.hpp>

#include "Nut/Scene/Scene.h"
#include "Nut/Scene/Entity.h"

namespace Nut
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);				// Must be used to Init Context!!! For updating Active Scene(which you are using)

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity& entity);
		void DrawComponents(Entity& entity);
		static void DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	private:
		Ref<Scene> m_Context;									// Active scene is the context, Entity (which is being used) is the selectionContext
		Entity m_SelectionContext;
	};


}