#pragma once

#include "entt.hpp"
#include "Nut/Core/Timestep.h"

namespace Nut
{
	class Entity;												//ǰ������

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);

		Entity CreateEntity(const std::string& name = "");

	private:
		entt::registry m_Registry;

		friend class Entity;
	};


}