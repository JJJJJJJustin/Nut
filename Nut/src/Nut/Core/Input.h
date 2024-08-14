#pragma once

#include "Nut\Core\Base.h"

namespace Nut
{
	class Input 
	{
	public:
		inline static bool IsKeyPressed(int keycode);

		inline static bool IsMouseButtonPressed(int button);
		inline static std::pair<float, float> GetMousePos();
		inline static float GetMouseX();
		inline static float GetMouseY();

	};
}