#include "nutpch.h"
#include "Scene.h"

#include "Nut/Scene/ScriptableEntity.h"
#include "Nut/Renderer/Renderer2D.h"
#include "Nut/Scene/Component.h"
#include "Nut/Scene/Entity.h"

#include "glm/glm.hpp"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"

namespace Nut
{
	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}
		NUT_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>(glm::vec3{ 0.0f });
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		m_Registry.destroy(entity);
	}

	// Init the Box2d world & Attach all physical properties to the object beforehand
	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for(auto e : view)
		{
			Entity entity = { e, this };								// Just for using member function of class Entity
			auto& tc = entity.GetComponent<TransformComponent>();
			auto& rb2c = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2c.Type);
			bodyDef.position.Set(tc.Translation.x, tc.Translation.y);
			bodyDef.angle = tc.Rotation.z;
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

			body->SetFixedRotation(rb2c.FixedRotation);
			rb2c.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2c = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2c.Size.x * tc.Scale.x, bc2c.Size.y * tc.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2c.Density;
				fixtureDef.friction = bc2c.Friction;
				fixtureDef.restitution = bc2c.Restitution;
				fixtureDef.restitutionThreshold = bc2c.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	// Destory the Box2d world
	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		{
			// Allow physics simulation & Update the body's transform every frame for rendering
			const int velocityIterations = 6;
			const int positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& tc = entity.GetComponent<TransformComponent>();
				auto& rb2c = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2c.RuntimeBody;
				b2Vec2 position = body->GetPosition();
				float angle = body->GetAngle();

				tc.Translation.x = position.x;
				tc.Translation.y = position.y;
				tc.Rotation.z = angle;
			}
		}

		// Render All 2D objects
		Camera* mainCamera = nullptr;
		glm::mat4 mainTransform;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			// ����⵽ĳ����������Ϊ����������򴫳�������������ݣ�Ȼ��������
			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = transform.GetTransform();
				break;
			}
		}

		if (mainCamera) {
			// Do some rendering (��ȡ��ǰ�������ͶӰ���� projection �� λ�ƾ��� transform��
			Renderer2D::BeginScene(*mainCamera, mainTransform);

			auto& group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// �����к��� TransformComponent ��ʵ�����Ѽ����� sprite ��ʵ�壬group ����һ������ע����ʵ�弯��
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}	
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera camera)
	{
		Renderer2D::BeginScene(camera);

		auto& group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);	// �����к��� TransformComponent ��ʵ�����Ѽ����� sprite ��ʵ�壬group ����һ������ע����ʵ�弯��
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);

			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}


	void Scene::OnScript(Timestep ts)
	{	
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		// ���С����нű������ʵ�塱����������� transform����Ϊ ScriptableEntity::GetComponent<>() ���� m_ScriptableEntity.GetComponent<T>(); 
		// ����� m_ScriptableEntity ���� Entity ���͵ģ����Ե��õ��� Entity ��GetComponent<T>()�������Ҫ�� m_ScriptableEntity ���г�ʼ����
		// Ϊ��ʹ�����������У��� Scene::OnScript �У�m_ScriptableEntity ����ʼ��Ϊ Entity{ entity, this }��
		// ���ڴ�ʱ�ڻص����� each �У�Entity{ entity, this }�ĵ�һ���������ɻص������Զ���ȡ�ģ����������������� erntity Ӧ�������ڴ����ʵ�壬Ҳ���Ǻ��нű������ʵ�塣

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
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
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
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.ViewportResize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
}