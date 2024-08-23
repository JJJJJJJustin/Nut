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
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "");

	private:
		entt::registry m_Registry;

		friend class Entity;
	};


}