#include "nutpch.h"
#include "Nut/Scene/SceneSerializer.h"

#include "Nut/Scene/Entity.h"


namespace Nut
{
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        :m_Scene(scene)
    {
    }

    void SceneSerializer::Serialize(const std::string& filepath)        // Creating a file-stream allows us write data to the file,so that we can store data outside
    {
        // Edit the content which will be sent to the file
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entites" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.view<entt::entity>().each([&](auto entityID) 
            {
                Entity entity{ entity, m_Scene.get() };
                if (!entity)
                    return;

                SerializeEntity(out, entity);
            });
        out << YAML::EndSeq;
        out << YAML::EndMap;
       
        // Send file-stream
        std::ofstream fout(filepath);   // Create and open a file from the filepath
        fout << out.c_str();            // Writing data
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        // Not implemented
        NUT_CORE_ASSERT(false, "SerializeRuntime is not implemented yet");
    }


    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        // Read file, and send it to string stream
        std::ifstream file(filepath);
        std::stringstream fileContent;
        fileContent << file.rdbuf();

        // Load file contents and initialize data
        YAML::Node data = YAML::Load(fileContent);
        if (!data["Scene"])
            return false;

        // Trace this process
        std::string sceneName = data["Scene"].as<std::string>();
        NUT_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        // According to data, we reproduce the scene
        auto entities = data["Entity"];
        if(entities)
        {
            for (auto entity : entities) 
            {
                // Reproduce codes
            }
        }
        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Not implemented
        NUT_CORE_ASSERT(false, "DeserializeRuntime is not implemented yet");
        return false;
    }

    // ---------------------------------------------------------
    // ------------------- Some definations --------------------
    // ---------------------------------------------------------
    void SceneSerializer::SerializeEntity(YAML::Emitter& out, const Entity& entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << "256257383941"; // TODO: Entity ID goes here
        // Wirte entity's data to YAML::emitter out
        out << YAML::EndMap;
    }
}