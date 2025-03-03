#pragma once

#include "Nut/Core/Timestep.h"
#include "Nut/Core/UUID.h"
#include "Nut/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;													//前向声明（针对Nut作用域以外的外部库：Box2D）

namespace Nut
{
	class Entity;												//前向声明（针对Nut作用域以内的类：Entity）

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithUUID(UUID id, const std::string& name = "");
		void DestroyEntity(Entity& entity);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera camera);
		void OnUpdateEditor(Timestep ts, EditorCamera camera);
		void OnScript(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);
	public:
		Entity GetPrimaryCameraEntity();

		static Ref<Scene> Copy(Ref<Scene> other);
		void DuplicateEntity(Entity& srcEntity);

		void RenderScene(EditorCamera& camera);

		void OnPhysics2DInit();
		void OnPhysics2DDestroy();
		void OnPhysics2DRun(Timestep& ts);

		template<typename... Components>
		auto GetEntityWithComponent()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;

		b2World* m_PhysicsWorld = nullptr;

		uint32_t m_ViewportWidth, m_ViewportHeight;			// maybe useful( We use it in Scene::OnComponentAdded<CameraComponent> )

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};


}