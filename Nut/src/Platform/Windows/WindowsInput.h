#pragma once

#include "Nut/Core/Input.h"

namespace Nut
{
	class WindowsInput : public Input			//������һ����̬�࣬��Ϊ���߼�ʹ�ã����Բ���ҪNUT_API����
	{
	protected:
		/*virtual*/ bool IsKeyPressedImpl(int keycode) override;

		/*virtual*/ bool IsMouseButtonPressedImpl(int button) override;
		/*virtual*/ std::pair<float, float> GetMousePosImpl() override;
		/*virtual*/ float GetMouseXImpl() override;
		/*virtual*/ float GetMouseYImpl() override;
	};
}