#include "nutpch.h"
#include "Scene.h"

#include "glm/glm.hpp"

namespace Nut
{

	static void DoMath(const glm::mat4& transform) 
	{
		// Do some math
	}

	Scene::Scene()
	{
		struct TransformComponent {
			glm::mat4 Transform;

			TransformComponent() = default;
			TransformComponent(const glm::mat4& trans)
				:Transform(trans) {}
			TransformComponent(const TransformComponent&) = default;				//���ƺ���
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

		auto group = m_Registry.group<TransformComponent, MeshComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
	}

	Scene::~Scene()
	{
	}
}