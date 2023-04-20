#pragma once
#include "Engine/Core.h"
#include <utility>

namespace Hanabi
{
	class Input
	{
	public:
		Input(const Input&) = delete;
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		Input& operator=(const Input&) = delete;
	protected:
		Input() = default;
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
	private:
		static Scope<Input> s_Instance;
	};
}