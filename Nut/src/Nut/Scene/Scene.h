#pragma once

#include "entt.hpp"

namespace Nut
{

	class Scene
	{
	public:
		Scene();
		~Scene();
	private:
		entt::registry m_Registry;
	};


}