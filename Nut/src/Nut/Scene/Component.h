#pragma once

#include <glm/glm.hpp>

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
		glm::mat4 Transform{ 1.0f };											// 矩阵默认初始化为 { 1.0f }

		TransformComponent() = default;
		TransformComponent(const glm::mat4& transform)
			:Transform(transform) {};
		TransformComponent(const TransformComponent&) = default;

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const glm::vec4& color)
			:Color(color) {};
		SpriteComponent(const SpriteComponent&) = default;

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	
	};


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
			InstantiateScript = []() { return new T(); };
			DeinstantiateScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};  // Why need to define nsc->Instance with nullptr?? 
		}
	};



}