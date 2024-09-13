#pragma once

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
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;									// Active scene is the context, Entity (which is being used) is the selectionContext
		Entity m_SelectionContext;
	};


}