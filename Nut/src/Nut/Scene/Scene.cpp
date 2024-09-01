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
		// Update scripts
		
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				bool cameraPrimary;
				auto cameraView = m_Registry.view<CameraComponent>();
				for (auto entity : cameraView)
				{
					cameraPrimary = cameraView.get<CameraComponent>(entity).Primary;
				}

				if (!nsc.Instance)
				{
					nsc.InstantiateFunction();
					nsc.Instance->m_ScriptableEntity = Entity{ entity, this };		// �ص������е�entity��һ��uint������id,����ҪΪm_ScriptableEntity���ù��캯��������id��Scene��ָ�롣

					if (nsc.OnCreateFunction)
						nsc.OnCreateFunction(nsc.Instance);
				}

				if (nsc.OnUpdateFunction && cameraPrimary) {
					nsc.OnUpdateFunction(nsc.Instance, ts);
					
				}
			});

		// Render 2D objects
		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			// ����⵽ĳ����������Ϊ����������򴫳�������������ݣ�Ȼ��������
			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = &transform.Transform;
				break;
			}
		}

		if (mainCamera) {
			// Do some rendering (��ȡ��ǰ�������ͶӰ���� projection �� λ�ƾ��� transform��
			Renderer2D::BeginScene(*mainCamera, glm::inverse(*mainTransform));

			auto& group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// �����к��� TransformComponent ��ʵ�����Ѽ����� sprite ��ʵ�壬group ����һ������ע����ʵ�弯��
			for (auto entity : group) {
				auto& [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.Transform, color.Color);
			}

			Renderer2D::EndScene();
		}

		
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto& view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = m_Registry.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.ViewportResize(width, height);
		}
	}



}