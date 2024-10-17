#pragma once

#include "Nut/Core/Timestep.h"
#include "Nut/Renderer/EditorCamera.h"

#include "entt.hpp"

namespace Nut
{
	class Entity;												//ǰ������

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity& entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera camera);
		void OnScript(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCamera();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;

		uint32_t m_ViewportWidth, m_ViewportHeight;			// maybe useful( We use it in Scene::OnComponentAdded<CameraComponent> )

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};


}