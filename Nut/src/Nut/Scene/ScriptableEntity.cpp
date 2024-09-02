#include "nutpch.h"
#include "Nut/Scene/ScriptableEntity.h"

#include "Nut/Scene/Component.h"

#include "Nut/Core/KeyCodes.h"
#include "Nut/Core/Input.h"

namespace Nut
{

	void ScriptableEntity::OnCreate()
	{}

	void ScriptableEntity::OnDestroy()
	{}

	void ScriptableEntity::OnUpdate(Timestep ts)
	{}

	// ---------------------------------------------------------------------------------------------------------------------

	void ScriptCameraController::OnCreate()
	{}

	void ScriptCameraController::OnDestroy()
	{}

	void ScriptCameraController::OnUpdate(Timestep ts)
	{
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		// 所有“含有脚本组件的实体”都将会更新其 transform，因为 ScriptableEntity::GetComponent<>() 返回 m_ScriptableEntity.GetComponent<T>(); 
		// 而这个 m_ScriptableEntity 又是 Entity 类型的，所以调用的是 Entity 的GetComponent<T>()，这就需要对 m_ScriptableEntity 进行初始化。
		// 为了使函数正常运行，在 Scene::OnScript 中，m_ScriptableEntity 被初始化为 Entity{ entity, this }。
		// 由于此时在回调函数 each 中，Entity{ entity, this }的第一个参数是有回调函数自动获取的，所以这个参数填入的 erntity 应该是正在处理的实体，也就是含有脚本组件的实体。

		// 所以说这将更新所有包含脚本组件的实体，而且每一个实体的 transform 改变的数值相同，这导致每一个实体都会受键盘影响而移动，尽管此时只有一个实体被显示出来。
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		auto& transform = GetComponent<TransformComponent>().Transform;
		float speed = 5.0f;

		if (Input::IsKeyPressed(NUT_KEY_A))
			transform[3][0] += speed * ts;
		if (Input::IsKeyPressed(NUT_KEY_D))
			transform[3][0] -= speed * ts;
		if (Input::IsKeyPressed(NUT_KEY_W))
			transform[3][1] -= speed * ts;
		if (Input::IsKeyPressed(NUT_KEY_S))
			transform[3][1] += speed * ts;
	}


}