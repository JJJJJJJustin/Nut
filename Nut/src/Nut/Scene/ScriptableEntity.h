#pragma once

#include "Nut/Scene/Entity.h"

namespace Nut 
{
	// ---------------------------------------------------------------------------------
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_ScriptableEntity.GetComponent<T>();
		}
	private:
		// Check the "m_ScriptableEntity.GetComponent();"
		// The member function which from "Entity" will use class "Scene",
		// So we declare Scene as a friend class.
		friend class Scene;	
		Entity m_ScriptableEntity;						//在 Scene::OnScript 中被初始化
	protected:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(Timestep ts) = 0;
	};

	// -----------------------------------------------------------------------------------
	class ScriptCameraController : public ScriptableEntity
	{
	public:
		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate(Timestep ts) override;
	};


}