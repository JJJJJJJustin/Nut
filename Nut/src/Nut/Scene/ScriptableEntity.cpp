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
		// ���С����нű������ʵ�塱����������� transform����Ϊ ScriptableEntity::GetComponent<>() ���� m_ScriptableEntity.GetComponent<T>(); 
		// ����� m_ScriptableEntity ���� Entity ���͵ģ����Ե��õ��� Entity ��GetComponent<T>()�������Ҫ�� m_ScriptableEntity ���г�ʼ����
		// Ϊ��ʹ�����������У��� Scene::OnScript �У�m_ScriptableEntity ����ʼ��Ϊ Entity{ entity, this }��
		// ���ڴ�ʱ�ڻص����� each �У�Entity{ entity, this }�ĵ�һ���������лص������Զ���ȡ�ģ����������������� erntity Ӧ�������ڴ����ʵ�壬Ҳ���Ǻ��нű������ʵ�塣

		// ����˵�⽫�������а����ű������ʵ�壬����ÿһ��ʵ��� transform �ı����ֵ��ͬ���⵼��ÿһ��ʵ�嶼���ܼ���Ӱ����ƶ������ܴ�ʱֻ��һ��ʵ�屻��ʾ������
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