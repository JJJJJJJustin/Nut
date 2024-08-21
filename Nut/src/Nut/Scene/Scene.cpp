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
		/*struct TransformComponent {
			glm::mat4 Transform;

			TransformComponent() = default;
			TransformComponent(const glm::mat4& trans)
				:Transform(trans) {}
			TransformComponent(const TransformComponent&) = default;				//���ƺ���

			operator glm::mat4& () { return Transform; }
			operator const glm::mat4& () const { return Transform; }
		};

		struct MeshComponent
		{
			int ID;

			MeshComponent() = default;
		};

		entt::entity Entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(Entity, glm::mat4(1.0f));

		if (m_Registry.try_get<TransformComponent>(Entity)) {
			
		}

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);					// ����ͼ�м���transform��������ܸߣ���ͼֻ�������ض������ʵ�壩
			//auto transform = m_Registry.get<TransformComponent>(entity);			// ��ע����м���transform������������������������ʵ�壩
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}*/

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
		// Do some rendering
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);		// �����к��� TransformComponent ��ʵ�����Ѽ����� sprite ��ʵ�壬group ����һ������ע����ʵ�弯��
		for (auto entity : group) {
			auto& [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

			Renderer2D::DrawQuad(transform, color);
		}
	}



}