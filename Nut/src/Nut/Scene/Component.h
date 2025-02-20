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

	struct CameraComponent
	{
		SceneCamera Camera;		// ToDo: think about moving to Scene
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	
	};

	// ---------------------- Script --------------------------------------
	class ScriptableEntity;								// ǰ��������forward declaration)

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		// Set functions( Use std::function )
		std::function<ScriptableEntity* ()> InstantiateScript;//ʵ�����ű�
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
		glm::vec2 Offset = { 0.0f, 0.0f };					// ����ƫ����
		glm::vec2 Size = { 0.5f, 0.5f };					// ��ײ���

		// TODO: May move this to Physics Material
		float Density = 1.0f;								// �ܶ�
		float Friction = 0.5f;								// Ħ����
		float Restitution = 0.0f;							// �ָ�
		float RestitutionThreshold = 0.5f;					// �ָ���ֵ

		// Storage for runtime
		void* RuntimeFixture = nullptr;						// ����ʱ�����������ʱ����

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};



}