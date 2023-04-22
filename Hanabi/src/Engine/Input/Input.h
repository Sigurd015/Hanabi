#pragma once
#include "Engine/Input/MouseCodes.h"
#include "Engine/Input/KeyCodes.h"

namespace Hanabi
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
	};
}