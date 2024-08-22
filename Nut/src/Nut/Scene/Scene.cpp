#include "nutpch.h"
#include "Scene.h"

#include "Nut/Renderer/Renderer2D.h"
#include "Nut/Scene/Component.h"
#include "Nut/Scene/Entity.h"

#include "glm/glm.hpp"

namespace Nut
{

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<TransformComponent>(glm::mat4{ 1.0f });
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			// 若检测到某摄像机被标记为主摄像机，则传出主摄像机的数据，然后跳出。
			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = &transform.Transform;
				break;
			}
		}

		if (mainCamera) {
			// Do some rendering (获取当前摄像机的投影矩阵 projection 和 位移矩阵 transform，
			Renderer2D::BeginScene(*mainCamera, inverse(*mainTransform));

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// 在所有含有 TransformComponent 的实体中搜集含有 sprite 的实体，group 返回一个类似注册表的实体集合
			for (auto entity : group) {
				auto& [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.Transform, color.Color);
			}

			Renderer2D::EndScene();
		}

	}



}