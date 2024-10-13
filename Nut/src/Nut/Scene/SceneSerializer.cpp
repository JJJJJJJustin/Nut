#include "nutpch.h"
#include "Nut/Scene/SceneSerializer.h"

#include "Nut/Scene/Entity.h"
#include "Nut/Scene/Component.h"

namespace YAML {
    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<double>();
            rhs.y = node[1].as<double>();
            rhs.z = node[2].as<double>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<double>();
            rhs.y = node[1].as<double>();
            rhs.z = node[2].as<double>();
            rhs.w = node[3].as<double>();
            return true;
        }
    };


}


namespace Nut
{
    YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3& vec)
    {
        out << YAML::Flow;                                              // Set the output format to streaming format instead of block format
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4& vec)       // We use it in "SerializeEntity() -> YAML::Value << color"
    {
        out << YAML::Flow;                                              // Set the output format to streaming format instead of block format
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
        return out;
    }


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
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.view<entt::entity>().each([&](auto entityID) 
            {
                Entity entity{ entityID, m_Scene.get() };
                if (!entity)
                    return;

                SerializeEntity(out, entity);
            });
        out << YAML::EndSeq;
        out << YAML::EndMap;
       
        // Send file-stream
        std::ofstream fout(filepath);                                   // Create and open a file from the filepath
        fout << out.c_str();                                            // Writing data
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
        auto entities = data["Entities"];
        if(entities)
        {
            for (auto entity : entities) 
            {
                uint64_t uuid;
                std::string name;

                uuid = entity["Entity"].as<uint64_t>();                         // TODO
                // Enter the TagComponent map, 
                // and search for tag in submap(submap is stroed in TagComponent map)
                auto tc = entity["TagComponent"];       
                if (tc)
                    name = tc["Tag"].as<std::string>();
                
                NUT_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
                Entity& deserializedEntity = m_Scene->CreateEntity(name);       // Create a new entity in m_Scene with all defalur values
                
                DeserializeEntity(entity, deserializedEntity);                  // Update values in this entity accroding to yaml file
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
    void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << "256257383941";          // TODO: Entity ID goes here
        
        if (entity.HasComponent<TagComponent>()) 
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<TransformComponent>()) 
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation"    << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale"       << YAML::Value << tc.Scale;
            
            out << YAML::EndMap;
        }

        if (entity.HasComponent<SpriteComponent>()) 
        {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap;

            auto& color = entity.GetComponent<SpriteComponent>().Color;
            out << YAML::Key << "Color" << YAML::Value << color;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<CameraComponent>()) 
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;

            auto& cc = entity.GetComponent<CameraComponent>(); 
            auto& camera = cc.Camera;
            
            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap;{
                out << YAML::Key << "ProjectionType"  << YAML::Value << (int)camera.GetProjectionType();
                out << YAML::Key << "PerspectiveFOV"  << YAML::Value << camera.GetPerspectiveVerticalFOV();
                out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
                out << YAML::Key << "PerspectiveFar"  << YAML::Value << camera.GetPerspectiveFarClip();
                out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
                out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
                out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.GetOrthographicFarClip();
                out << YAML::EndMap;
            }
            out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

            out << YAML::EndMap;
        
        }

        out << YAML::EndMap;
    }

    void SceneSerializer::DeserializeEntity(YAML::detail::iterator_value& data, Entity& entity)
    {
        // Now transformComponent is the map 
        // which was stroed all TransformComponent data(such as scale,rotation...)
        auto transformComponent = data["TransformComponent"]; 
        if(transformComponent)
        {
            auto& tc = entity.GetComponent<TransformComponent>();
            tc.Translation = transformComponent["Translation"].as<glm::vec3>();
            tc.Translation = transformComponent["Rotation"].as<glm::vec3>();
            tc.Translation = transformComponent["Scale"].as<glm::vec3>();
        }

        auto cameraComponent = data["CameraComponent"];
        if(cameraComponent)
        {
            auto& cameraProps = cameraComponent["Camera"];                                     // One of the submap of camera component
            auto& cc = entity.AddComponent<CameraComponent>();

            cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
            cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
            cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
            cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
            cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
            cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
            cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
            // Unlike Camera, Primary is a separate key-value mapping, 
            // while Camera is a map that requires further access.
            cc.Primary = cameraComponent["Primary"].as<bool>();
            cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
        }

        auto spriteComponent = data["SpriteComponent"];
        if (spriteComponent)
        {
            auto& sc = entity.AddComponent<SpriteComponent>();
            sc.Color = spriteComponent["Color"].as<glm::vec4>();
        }
    }



}