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

		void SetContext(const Ref<Scene>& scene);				// Change Active Scene(which you are using)

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		// Entity m_SelectionContext;
	};


}