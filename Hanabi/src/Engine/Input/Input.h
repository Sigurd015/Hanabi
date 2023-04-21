#pragma once
#include "Engine/Core.h"
#include <utility>
#include "Engine/Input/MouseCodes.h"
#include "Engine/Input/KeyCodes.h"

namespace Hanabi
{
	class Input
	{
	public:
		Input(const Input&) = delete;
		static bool IsKeyPressed(KeyCode key) { return s_Instance->IsKeyPressedImpl(key); }
		static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		Input& operator=(const Input&) = delete;
		static Scope<Input> Create();
	protected:
		Input() = default;
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
	private:
		static Scope<Input> s_Instance;
	};
}