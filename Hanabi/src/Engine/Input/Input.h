#pragma once
#include "KeyCodes.h"
#include "KeyCodes.h"
#include <glm/glm.hpp>

namespace Hanabi
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseButton button);
		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);
		static CursorMode GetCursorMode();
	};
}