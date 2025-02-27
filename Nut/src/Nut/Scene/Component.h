#pragma once

#include "Nut/Core/UUID.h"
#include "Nut/Renderer/Texture.h"
#include "Nut/Scene/SceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace Nut
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const UUID& id)
			:ID(id) {};
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {};
		TagComponent(const TagComponent&) = default;

	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)		// TODO: Maybe it needs to take type glm::mat4 as parameter ?
			:Translation(translation) {};
		TransformComponent(const TransformComponent&) = default;

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteComponent() = default;
		SpriteComponent(const glm::vec4& color)
			:Color(color) {};
		SpriteComponent(const SpriteComponent&) = default;

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};

	struct CircleComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleComponent() = default;
		CircleComponent(const glm::vec4& color)
			:Color(color) {};
		CircleComponent(const CircleComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;		// ToDo: think about moving to Scene
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	
	};

	// ---------------------- Script --------------------------------------
	class ScriptableEntity;								// 前向声明（forward declaration)

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		// Set functions( Use std::function )
		std::function<ScriptableEntity* ()> InstantiateScript;//实例化脚本
		std::function<void(NativeScriptComponent*)> DeinstantiateScript;
		/* Set functions( Use function pointer ) 
		   ( Both of them are correct, but function pointer will faster, and better at performance )
		
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);*/


		// Send functions so we can use them later
		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DeinstantiateScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};  // Why need to define nsc->Instance with nullptr?? 
		}
	};


	// ----------------------- Physics --------------------------------------
	struct Rigidbody2DComponent
	{
		enum class BodyType{ Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime (just for runtime, don't need to serialize)
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };					// 重心偏移量
		glm::vec2 Size = { 0.5f, 0.5f };					// 碰撞体积大小(x轴增量，y轴增量）

		// TODO: May move this to Physics Material
		float Density = 1.0f;								// 密度
		float Friction = 0.5f;								// 摩擦力
		float Restitution = 0.0f;							// 恢复
		float RestitutionThreshold = 0.5f;					// 恢复阈值

		// Storage for runtime
		void* RuntimeFixture = nullptr;						// 运行时的配件、运行时附件

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };					// 重心偏移量
		float Radius = 0.5f;								// 碰撞半径大小

		// TODO: May move this to Physics Material
		float Density = 1.0f;								// 密度
		float Friction = 0.5f;								// 摩擦力
		float Restitution = 0.0f;							// 恢复
		float RestitutionThreshold = 0.5f;					// 恢复阈值

		// Storage for runtime
		void* RuntimeFixture = nullptr;						// 运行时的配件、运行时附件

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

}