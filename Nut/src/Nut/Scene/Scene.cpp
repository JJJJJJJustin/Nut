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
		// Render 2D objects
		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
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
				auto [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.Transform, color.Color);
			}

			Renderer2D::EndScene();
		}	
	}


	void Scene::OnScript(Timestep ts)
	{	
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		// ���С����нű������ʵ�塱����������� transform����Ϊ ScriptableEntity::GetComponent<>() ���� m_ScriptableEntity.GetComponent<T>(); 
		// ����� m_ScriptableEntity ���� Entity ���͵ģ����Ե��õ��� Entity ��GetComponent<T>()�������Ҫ�� m_ScriptableEntity ���г�ʼ����
		// Ϊ��ʹ�����������У��� Scene::OnScript �У�m_ScriptableEntity ����ʼ��Ϊ Entity{ entity, this }��
		// ���ڴ�ʱ�ڻص����� each �У�Entity{ entity, this }�ĵ�һ���������лص������Զ���ȡ�ģ����������������� erntity Ӧ�������ڴ����ʵ�壬Ҳ���Ǻ��нű������ʵ�塣

		// ����˵�⽫�������а����ű������ʵ�壬����ÿһ��ʵ��� transform �ı����ֵ��ͬ���⵼��ÿһ��ʵ�嶼���ܼ���Ӱ����ƶ������ܴ�ʱֻ��һ��ʵ�屻��ʾ������
		// --------------------------------------------------------------------------------------------------------------------------------------------------------


		// Update scripts�������к��� NativeScriptComponent ��������д�������ʽ�� lambda ���壩
		m_Registry.view<NativeScriptComponent, CameraComponent>().each
		(
			[=](auto entity, auto& nsc, auto& cc)								// nsc => NativeScriptController, cc => CameraController
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_ScriptableEntity = Entity{ entity, this };	// �ص������е�entity��һ��uint������id,����ҪΪm_ScriptableEntity���ù��캯��������id��Scene��ָ�롣

					nsc.Instance->OnCreate();
				}

				if(cc.Primary)
					nsc.Instance->OnUpdate(ts);
			}
		);
	
	
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