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

		Entity CreateEntity(const std::string& name = "");

		// TEMP
		entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;
	};


}