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
			// ����⵽ĳ����������Ϊ����������򴫳�������������ݣ�Ȼ��������
			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = &transform.Transform;
				break;
			}
		}

		if (mainCamera) {
			// Do some rendering (��ȡ��ǰ�������ͶӰ���� projection �� λ�ƾ��� transform��
			Renderer2D::BeginScene(*mainCamera, inverse(*mainTransform));

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// �����к��� TransformComponent ��ʵ�����Ѽ����� sprite ��ʵ�壬group ����һ������ע����ʵ�弯��
			for (auto entity : group) {
				auto& [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.Transform, color.Color);
			}

			Renderer2D::EndScene();
		}

	}



}