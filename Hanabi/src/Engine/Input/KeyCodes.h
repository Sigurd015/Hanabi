#pragma once

namespace Hanabi
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define HNB_KEY_SPACE              32
#define HNB_KEY_APOSTROPHE         39  /* ' */
#define HNB_KEY_COMMA              44  /* , */
#define HNB_KEY_MINUS              45  /* - */
#define HNB_KEY_PERIOD             46  /* . */
#define HNB_KEY_SLASH              47  /* / */
#define HNB_KEY_0                  48
#define HNB_KEY_1                  49
#define HNB_KEY_2                  50
#define HNB_KEY_3                  51
#define HNB_KEY_4                  52
#define HNB_KEY_5                  53
#define HNB_KEY_6                  54
#define HNB_KEY_7                  55
#define HNB_KEY_8                  56
#define HNB_KEY_9                  57
#define HNB_KEY_SEMICOLON          59  /* ; */
#define HNB_KEY_EQUAL              61  /* = */
#define HNB_KEY_A                  65
#define HNB_KEY_B                  66
#define HNB_KEY_C                  67
#define HNB_KEY_D                  68
#define HNB_KEY_E                  69
#define HNB_KEY_F                  70
#define HNB_KEY_G                  71
#define HNB_KEY_H                  72
#define HNB_KEY_I                  73
#define HNB_KEY_J                  74
#define HNB_KEY_K                  75
#define HNB_KEY_L                  76
#define HNB_KEY_M                  77
#define HNB_KEY_N                  78
#define HNB_KEY_O                  79
#define HNB_KEY_P                  80
#define HNB_KEY_Q                  81
#define HNB_KEY_R                  82
#define HNB_KEY_S                  83
#define HNB_KEY_T                  84
#define HNB_KEY_U                  85
#define HNB_KEY_V                  86
#define HNB_KEY_W                  87
#define HNB_KEY_X                  88
#define HNB_KEY_Y                  89
#define HNB_KEY_Z                  90
#define HNB_KEY_LEFT_BRACKET       91  /* [ */
#define HNB_KEY_BACKSLASH          92  /* \ */
#define HNB_KEY_RIGHT_BRACKET      93  /* ] */
#define HNB_KEY_GRAVE_ACCENT       96  /* ` */
#define HNB_KEY_WORLD_1            161 /* non-US #1 */
#define HNB_KEY_WORLD_2            162 /* non-US #2 */
#define HNB_KEY_SPACE           ::Hanabi::Key::Space
#define HNB_KEY_APOSTROPHE      ::Hanabi::Key::Apostrophe    /* ' */
#define HNB_KEY_COMMA           ::Hanabi::Key::Comma         /* , */
#define HNB_KEY_MINUS           ::Hanabi::Key::Minus         /* - */
#define HNB_KEY_PERIOD          ::Hanabi::Key::Period        /* . */
#define HNB_KEY_SLASH           ::Hanabi::Key::Slash         /* / */
#define HNB_KEY_0               ::Hanabi::Key::D0
#define HNB_KEY_1               ::Hanabi::Key::D1
#define HNB_KEY_2               ::Hanabi::Key::D2
#define HNB_KEY_3               ::Hanabi::Key::D3
#define HNB_KEY_4               ::Hanabi::Key::D4
#define HNB_KEY_5               ::Hanabi::Key::D5
#define HNB_KEY_6               ::Hanabi::Key::D6
#define HNB_KEY_7               ::Hanabi::Key::D7
#define HNB_KEY_8               ::Hanabi::Key::D8
#define HNB_KEY_9               ::Hanabi::Key::D9
#define HNB_KEY_SEMICOLON       ::Hanabi::Key::Semicolon     /* ; */
#define HNB_KEY_EQUAL           ::Hanabi::Key::Equal         /* = */
#define HNB_KEY_A               ::Hanabi::Key::A
#define HNB_KEY_B               ::Hanabi::Key::B
#define HNB_KEY_C               ::Hanabi::Key::C
#define HNB_KEY_D               ::Hanabi::Key::D
#define HNB_KEY_E               ::Hanabi::Key::E
#define HNB_KEY_F               ::Hanabi::Key::F
#define HNB_KEY_G               ::Hanabi::Key::G
#define HNB_KEY_H               ::Hanabi::Key::H
#define HNB_KEY_I               ::Hanabi::Key::I
#define HNB_KEY_J               ::Hanabi::Key::J
#define HNB_KEY_K               ::Hanabi::Key::K
#define HNB_KEY_L               ::Hanabi::Key::L
#define HNB_KEY_M               ::Hanabi::Key::M
#define HNB_KEY_N               ::Hanabi::Key::N
#define HNB_KEY_O               ::Hanabi::Key::O
#define HNB_KEY_P               ::Hanabi::Key::P
#define HNB_KEY_Q               ::Hanabi::Key::Q
#define HNB_KEY_R               ::Hanabi::Key::R
#define HNB_KEY_S               ::Hanabi::Key::S
#define HNB_KEY_T               ::Hanabi::Key::T
#define HNB_KEY_U               ::Hanabi::Key::U
#define HNB_KEY_V               ::Hanabi::Key::V
#define HNB_KEY_W               ::Hanabi::Key::W
#define HNB_KEY_X               ::Hanabi::Key::X
#define HNB_KEY_Y               ::Hanabi::Key::Y
#define HNB_KEY_Z               ::Hanabi::Key::Z
#define HNB_KEY_LEFT_BRACKET    ::Hanabi::Key::LeftBracket   /* [ */
#define HNB_KEY_BACKSLASH       ::Hanabi::Key::Backslash     /* \ */
#define HNB_KEY_RIGHT_BRACKET   ::Hanabi::Key::RightBracket  /* ] */
#define HNB_KEY_GRAVE_ACCENT    ::Hanabi::Key::GraveAccent   /* ` */
#define HNB_KEY_WORLD_1         ::Hanabi::Key::World1        /* non-US #1 */
#define HNB_KEY_WORLD_2         ::Hanabi::Key::World2        /* non-US #2 */

/* Function keys */
#define HNB_KEY_ESCAPE             256
#define HNB_KEY_ENTER              257
#define HNB_KEY_TAB                258
#define HNB_KEY_BACKSPACE          259
#define HNB_KEY_INSERT             260
#define HNB_KEY_DELETE             261
#define HNB_KEY_RIGHT              262
#define HNB_KEY_LEFT               263
#define HNB_KEY_DOWN               264
#define HNB_KEY_UP                 265
#define HNB_KEY_PAGE_UP            266
#define HNB_KEY_PAGE_DOWN          267
#define HNB_KEY_HOME               268
#define HNB_KEY_END                269
#define HNB_KEY_CAPS_LOCK          280
#define HNB_KEY_SCROLL_LOCK        281
#define HNB_KEY_NUM_LOCK           282
#define HNB_KEY_PRINT_SCREEN       283
#define HNB_KEY_PAUSE              284
#define HNB_KEY_F1                 290
#define HNB_KEY_F2                 291
#define HNB_KEY_F3                 292
#define HNB_KEY_F4                 293
#define HNB_KEY_F5                 294
#define HNB_KEY_F6                 295
#define HNB_KEY_F7                 296
#define HNB_KEY_F8                 297
#define HNB_KEY_F9                 298
#define HNB_KEY_F10                299
#define HNB_KEY_F11                300
#define HNB_KEY_F12                301
#define HNB_KEY_F13                302
#define HNB_KEY_F14                303
#define HNB_KEY_F15                304
#define HNB_KEY_F16                305
#define HNB_KEY_F17                306
#define HNB_KEY_F18                307
#define HNB_KEY_F19                308
#define HNB_KEY_F20                309
#define HNB_KEY_F21                310
#define HNB_KEY_F22                311
#define HNB_KEY_F23                312
#define HNB_KEY_F24                313
#define HNB_KEY_F25                314
#define HNB_KEY_KP_0               320
#define HNB_KEY_KP_1               321
#define HNB_KEY_KP_2               322
#define HNB_KEY_KP_3               323
#define HNB_KEY_KP_4               324
#define HNB_KEY_KP_5               325
#define HNB_KEY_KP_6               326
#define HNB_KEY_KP_7               327
#define HNB_KEY_KP_8               328
#define HNB_KEY_KP_9               329
#define HNB_KEY_KP_DECIMAL         330
#define HNB_KEY_KP_DIVIDE          331
#define HNB_KEY_KP_MULTIPLY        332
#define HNB_KEY_KP_SUBTRACT        333
#define HNB_KEY_KP_ADD             334
#define HNB_KEY_KP_ENTER           335
#define HNB_KEY_KP_EQUAL           336
#define HNB_KEY_LEFT_SHIFT         340
#define HNB_KEY_LEFT_CONTROL       341
#define HNB_KEY_LEFT_ALT           342
#define HNB_KEY_LEFT_SUPER         343
#define HNB_KEY_RIGHT_SHIFT        344
#define HNB_KEY_RIGHT_CONTROL      345
#define HNB_KEY_RIGHT_ALT          346
#define HNB_KEY_RIGHT_SUPER        347
#define HNB_KEY_MENU               348
#define HNB_KEY_ESCAPE          ::Hanabi::Key::Escape
#define HNB_KEY_ENTER           ::Hanabi::Key::Enter
#define HNB_KEY_TAB             ::Hanabi::Key::Tab
#define HNB_KEY_BACKSPACE       ::Hanabi::Key::Backspace
#define HNB_KEY_INSERT          ::Hanabi::Key::Insert
#define HNB_KEY_DELETE          ::Hanabi::Key::Delete
#define HNB_KEY_RIGHT           ::Hanabi::Key::Right
#define HNB_KEY_LEFT            ::Hanabi::Key::Left
#define HNB_KEY_DOWN            ::Hanabi::Key::Down
#define HNB_KEY_UP              ::Hanabi::Key::Up
#define HNB_KEY_PAGE_UP         ::Hanabi::Key::PageUp
#define HNB_KEY_PAGE_DOWN       ::Hanabi::Key::PageDown
#define HNB_KEY_HOME            ::Hanabi::Key::Home
#define HNB_KEY_END             ::Hanabi::Key::End
#define HNB_KEY_CAPS_LOCK       ::Hanabi::Key::CapsLock
#define HNB_KEY_SCROLL_LOCK     ::Hanabi::Key::ScrollLock
#define HNB_KEY_NUM_LOCK        ::Hanabi::Key::NumLock
#define HNB_KEY_PRINT_SCREEN    ::Hanabi::Key::PrintScreen
#define HNB_KEY_PAUSE           ::Hanabi::Key::Pause
#define HNB_KEY_F1              ::Hanabi::Key::F1
#define HNB_KEY_F2              ::Hanabi::Key::F2
#define HNB_KEY_F3              ::Hanabi::Key::F3
#define HNB_KEY_F4              ::Hanabi::Key::F4
#define HNB_KEY_F5              ::Hanabi::Key::F5
#define HNB_KEY_F6              ::Hanabi::Key::F6
#define HNB_KEY_F7              ::Hanabi::Key::F7
#define HNB_KEY_F8              ::Hanabi::Key::F8
#define HNB_KEY_F9              ::Hanabi::Key::F9
#define HNB_KEY_F10             ::Hanabi::Key::F10
#define HNB_KEY_F11             ::Hanabi::Key::F11
#define HNB_KEY_F12             ::Hanabi::Key::F12
#define HNB_KEY_F13             ::Hanabi::Key::F13
#define HNB_KEY_F14             ::Hanabi::Key::F14
#define HNB_KEY_F15             ::Hanabi::Key::F15
#define HNB_KEY_F16             ::Hanabi::Key::F16
#define HNB_KEY_F17             ::Hanabi::Key::F17
#define HNB_KEY_F18             ::Hanabi::Key::F18
#define HNB_KEY_F19             ::Hanabi::Key::F19
#define HNB_KEY_F20             ::Hanabi::Key::F20
#define HNB_KEY_F21             ::Hanabi::Key::F21
#define HNB_KEY_F22             ::Hanabi::Key::F22
#define HNB_KEY_F23             ::Hanabi::Key::F23
#define HNB_KEY_F24             ::Hanabi::Key::F24
#define HNB_KEY_F25             ::Hanabi::Key::F25

/* Keypad */
#define HNB_KEY_KP_0            ::Hanabi::Key::KP0
#define HNB_KEY_KP_1            ::Hanabi::Key::KP1
#define HNB_KEY_KP_2            ::Hanabi::Key::KP2
#define HNB_KEY_KP_3            ::Hanabi::Key::KP3
#define HNB_KEY_KP_4            ::Hanabi::Key::KP4
#define HNB_KEY_KP_5            ::Hanabi::Key::KP5
#define HNB_KEY_KP_6            ::Hanabi::Key::KP6
#define HNB_KEY_KP_7            ::Hanabi::Key::KP7
#define HNB_KEY_KP_8            ::Hanabi::Key::KP8
#define HNB_KEY_KP_9            ::Hanabi::Key::KP9
#define HNB_KEY_KP_DECIMAL      ::Hanabi::Key::KPDecimal
#define HNB_KEY_KP_DIVIDE       ::Hanabi::Key::KPDivide
#define HNB_KEY_KP_MULTIPLY     ::Hanabi::Key::KPMultiply
#define HNB_KEY_KP_SUBTRACT     ::Hanabi::Key::KPSubtract
#define HNB_KEY_KP_ADD          ::Hanabi::Key::KPAdd
#define HNB_KEY_KP_ENTER        ::Hanabi::Key::KPEnter
#define HNB_KEY_KP_EQUAL        ::Hanabi::Key::KPEqual

#define HNB_KEY_LEFT_SHIFT      ::Hanabi::Key::LeftShift
#define HNB_KEY_LEFT_CONTROL    ::Hanabi::Key::LeftControl
#define HNB_KEY_LEFT_ALT        ::Hanabi::Key::LeftAlt
#define HNB_KEY_LEFT_SUPER      ::Hanabi::Key::LeftSuper
#define HNB_KEY_RIGHT_SHIFT     ::Hanabi::Key::RightShift
#define HNB_KEY_RIGHT_CONTROL   ::Hanabi::Key::RightControl
#define HNB_KEY_RIGHT_ALT       ::Hanabi::Key::RightAlt
#define HNB_KEY_RIGHT_SUPER     ::Hanabi::Key::RightSuper
#define HNB_KEY_MENU            ::Hanabi::Key::Menu