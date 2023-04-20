#pragma once

namespace Hanabi
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define HNB_MOUSE_BUTTON_0      ::Hanabi::Mouse::Button0
#define HNB_MOUSE_BUTTON_1      ::Hanabi::Mouse::Button1
#define HNB_MOUSE_BUTTON_2      ::Hanabi::Mouse::Button2
#define HNB_MOUSE_BUTTON_3      ::Hanabi::Mouse::Button3
#define HNB_MOUSE_BUTTON_4      ::Hanabi::Mouse::Button4
#define HNB_MOUSE_BUTTON_5      ::Hanabi::Mouse::Button5
#define HNB_MOUSE_BUTTON_6      ::Hanabi::Mouse::Button6
#define HNB_MOUSE_BUTTON_7      ::Hanabi::Mouse::Button7
#define HNB_MOUSE_BUTTON_LAST   ::Hanabi::Mouse::ButtonLast
#define HNB_MOUSE_BUTTON_LEFT   ::Hanabi::Mouse::ButtonLeft
#define HNB_MOUSE_BUTTON_RIGHT  ::Hanabi::Mouse::ButtonRight
#define HNB_MOUSE_BUTTON_MIDDLE ::Hanabi::Mouse::ButtonMiddle