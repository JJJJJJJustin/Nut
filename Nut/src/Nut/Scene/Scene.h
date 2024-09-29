#pragma once

#include "entt.hpp"
#include "Nut/Core/Timestep.h"

namespace Nut
{
	class Entity;												//Ç°ÏòÉùÃ÷

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);
		void OnScript(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity& entity);

	private:
		entt::registry m_Registry;

		uint32_t m_ViewportWidth, m_ViewportHeight;			// maybe useful

		friend class Entity;
		friend class SceneHierarchyPanel;
	};


}