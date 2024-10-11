#pragma once

#include "Nut/Scene/Scene.h"

#include <yaml-cpp/yaml.h>

namespace Nut
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);

		void SerializeEntity(YAML::Emitter& out, const Entity& entity);
	private:
		Ref<Scene> m_Scene;
	};


}