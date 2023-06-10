#pragma once
#include "MouseCodes.h"
#include "KeyCodes.h"
#include <glm/glm.hpp>

namespace Hanabi
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
	};
}