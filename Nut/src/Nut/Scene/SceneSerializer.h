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
	private:
		void SerializeEntity(YAML::Emitter& out, Entity entity);
		void DeserializeEntity(YAML::detail::iterator_value& data, Entity& entity);
	private:
		Ref<Scene> m_Scene;
	};


}