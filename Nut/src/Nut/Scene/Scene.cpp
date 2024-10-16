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

		entity.AddComponent<TransformComponent>(glm::vec3{ 0.0f });
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Render 2D objects
		Camera* mainCamera = nullptr;
		glm::mat4 mainTransform;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			// 若检测到某摄像机被标记为主摄像机，则传出主摄像机的数据，然后跳出。
			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = transform.GetTransform();
				break;
			}
		}

		if (mainCamera) {
			// Do some rendering (获取当前摄像机的投影矩阵 projection 和 位移矩阵 transform，
			Renderer2D::BeginScene(*mainCamera, glm::inverse(mainTransform));

			auto& group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// 在所有含有 TransformComponent 的实体中搜集含有 sprite 的实体，group 返回一个类似注册表的实体集合
			for (auto entity : group) {
				auto [transform, color] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), color.Color);
			}

			Renderer2D::EndScene();
		}	
	}


	void Scene::OnScript(Timestep ts)
	{	
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		// 所有“含有脚本组件的实体”都将会更新其 transform，因为 ScriptableEntity::GetComponent<>() 返回 m_ScriptableEntity.GetComponent<T>(); 
		// 而这个 m_ScriptableEntity 又是 Entity 类型的，所以调用的是 Entity 的GetComponent<T>()，这就需要对 m_ScriptableEntity 进行初始化。
		// 为了使函数正常运行，在 Scene::OnScript 中，m_ScriptableEntity 被初始化为 Entity{ entity, this }。
		// 由于此时在回调函数 each 中，Entity{ entity, this }的第一个参数是由回调函数自动获取的，所以这个参数填入的 erntity 应该是正在处理的实体，也就是含有脚本组件的实体。

		// 所以说这将更新所有包含脚本组件的实体，而且每一个实体的 transform 改变的数值相同，这导致每一个实体都会受键盘影响而移动，尽管此时只有一个实体被显示出来。
		// --------------------------------------------------------------------------------------------------------------------------------------------------------


		// Update scripts（对所有含有 NativeScriptComponent 的组件进行处理，处理方式由 lambda 定义）
		m_Registry.view<NativeScriptComponent, CameraComponent>().each
		(
			[=](auto entity, auto& nsc, auto& cc)								// nsc => NativeScriptController, cc => CameraController
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_ScriptableEntity = Entity{ entity, this };	// 回调函数中的entity是一个uint，记载id,故需要为m_ScriptableEntity调用构造函数，传入id和Scene的指针。

					nsc.Instance->OnCreate();
				}

				/*if(cc.Primary)*/
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


	Entity Scene::GetPrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) 
		{
			const auto& cameraComponent = m_Registry.get<CameraComponent>(entity);
			if (cameraComponent.Primary == true)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) 
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.ViewportResize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}