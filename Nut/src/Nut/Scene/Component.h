#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "Nut/Renderer/Texture.h"
#include "Nut/Scene/SceneCamera.h"
#include "Nut/Scene/ScriptableEntity.h"

namespace Nut
{

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


	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		// Set functions( Use std::function )
		std::function<ScriptableEntity* ()> InstantiateScript;//ÊµÀý»¯½Å±¾
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



}