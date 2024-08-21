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
			TransformComponent(const TransformComponent&) = default;				//复制函数

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
			auto& transform = view.get<TransformComponent>(entity);					// 在视图中检索transform组件，性能高（视图只包含了特定组件的实体）
			//auto transform = m_Registry.get<TransformComponent>(entity);			// 在注册表中检索transform组件，性能弱（祖册表包含所有实体）
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
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);		// 在所有含有 TransformComponent 的实体中搜集含有 sprite 的实体，group 返回一个类似注册表的实体集合
		for (auto entity : group) {
			auto& [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

			Renderer2D::DrawQuad(transform, color);
		}
	}



}