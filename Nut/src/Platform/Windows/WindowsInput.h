#pragma once

#include "Nut/Core/Input.h"

namespace Nut
{
	class WindowsInput : public Input			//该类是一个静态类，作为工具集使用，所以不需要NUT_API导出
	{
	protected:
		/*virtual*/ bool IsKeyPressedImpl(int keycode) override;

		/*virtual*/ bool IsMouseButtonPressedImpl(int button) override;
		/*virtual*/ std::pair<float, float> GetMousePosImpl() override;
		/*virtual*/ float GetMouseXImpl() override;
		/*virtual*/ float GetMouseYImpl() override;
	};
}