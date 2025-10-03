#pragma once
#include "Core/Tools/Macros.h"

namespace Iris
{
	namespace Engine
	{
		enum class InputState : unsigned int
		{
			RELEASE = 0,
			PRESS = 1,
			REPEAT = 2
		};

		enum class IrisInput : unsigned int
		{
			// === Mouse ===
			IE_MOUSEBUTTON_1 = 0,
			IE_MOUSEBUTTON_2 = 1,
			IE_MOUSEBUTTON_3 = 2,
			IE_MOUSEBUTTON_4 = 3,
			IE_MOUSEBUTTON_5 = 4,
			IE_MOUSEBUTTON_6 = 5,
			IE_MOUSEBUTTON_7 = 6,
			IE_MOUSEBUTTON_8 = 7,
			IE_MOUSEBUTTON_LEFT = IE_MOUSEBUTTON_1,
			IE_MOUSEBUTTON_RIGHT = IE_MOUSEBUTTON_2,
			IE_MOUSEBUTTON_MIDDLE = IE_MOUSEBUTTON_3,

			// === TEXT ===
			IE_KEY_A = 8,
			IE_KEY_B = 9,
			IE_KEY_C = 10,
			IE_KEY_D = 11,
			IE_KEY_E = 12,
			IE_KEY_F = 13,
			IE_KEY_G = 14,
			IE_KEY_H = 15,
			IE_KEY_I = 16,
			IE_KEY_J = 17,
			IE_KEY_K = 18,
			IE_KEY_L = 19,
			IE_KEY_M = 20,
			IE_KEY_N = 21,
			IE_KEY_O = 22,
			IE_KEY_P = 23,
			IE_KEY_Q = 24,
			IE_KEY_R = 25,
			IE_KEY_S = 26,
			IE_KEY_T = 27,
			IE_KEY_U = 28,
			IE_KEY_V = 29,
			IE_KEY_W = 30,
			IE_KEY_X = 31,
			IE_KEY_Y = 32,
			IE_KEY_Z = 33,

			// === SPECIAL ===
			IE_KEY_SPACE = 34,
			IE_KEY_ARROW_RIGHT = 35,
			IE_KEY_ARROW_LEFT = 36,
			IE_KEY_ARROW_DOWN = 37,
			IE_KEY_ARROW_UP = 38,
			IE_KEY_BACKSPACE = 39,
			IE_KEY_ESCAPE = 40,
			IE_KEY_ENTER = 41,
			IE_KEY_TAB = 42,
			IE_KEY_CAPSLOCK = 43,
			IE_KEY_LEFT_SHIFT = 44,
			IE_KEY_LEFT_CONTROL = 45,
			IE_KEY_LEFT_ALT = 46,
			IE_KEY_RIGHT_SHIFT = 47,
			IE_KEY_RIGHT_CONTROL = 48,
			IE_KEY_RIGHT_ALT = 49,

			// === NUMBERS ===
			IE_KEY_0 = 50,
			IE_KEY_1 = 51,
			IE_KEY_2 = 52,
			IE_KEY_3 = 53,
			IE_KEY_4 = 54,
			IE_KEY_5 = 55,
			IE_KEY_6 = 56,
			IE_KEY_7 = 57,
			IE_KEY_8 = 58,
			IE_KEY_9 = 59,

			// === KEY PAD ===
			IE_KEY_KP0 = 60,
			IE_KEY_KP1 = 61,
			IE_KEY_KP2 = 62,
			IE_KEY_KP3 = 63,
			IE_KEY_KP4 = 64,
			IE_KEY_KP5 = 65,
			IE_KEY_KP6 = 66,
			IE_KEY_KP7 = 67,
			IE_KEY_KP8 = 68,
			IE_KEY_KP9 = 69,
			IE_KEY_KP_DECIMAL = 70,
			IE_KEY_KP_DIVIDE = 71,
			IE_KEY_KP_MULTIPLY = 72,
			IE_KEY_KP_SUBSTRACT = 73,
			IE_KEY_KP_ADD = 74,
			IE_KEY_KP_ENTER = 75,
			IE_KEY_KP_EQUAL = 76,

			// === SPECIAL SYMBOLE ===
			IE_KEY_GRAVEACCENT = 77,
			IE_KEY_MINUS = 78,
			IE_KEY_EQUAL = 79,
			IE_KEY_LEFTBRACKET = 80,
			IE_KEY_RIGHTBRACKET = 81,
			IE_KEY_BACKSLASH = 82,
			IE_KEY_SEMICOLON = 83,
			IE_KEY_APOSTROPHE = 84,
			IE_KEY_COMMA = 85,
			IE_KEY_PERIOD = 86,
			IE_KEY_SLASH = 87,

			// === F ===
			IE_KEY_F1 = 88,
			IE_KEY_F2 = 89,
			IE_KEY_F3 = 90,
			IE_KEY_F4 = 91,
			IE_KEY_F5 = 92,
			IE_KEY_F6 = 93,
			IE_KEY_F7 = 94,
			IE_KEY_F8 = 95,
			IE_KEY_F9 = 96,
			IE_KEY_F10 = 97,
			IE_KEY_F11 = 98,
			IE_KEY_F12 = 99,
			IE_KEY_F13 = 100,
			IE_KEY_F14 = 101,
			IE_KEY_F15 = 102,
			IE_KEY_F16 = 103,
			IE_KEY_F17 = 104,
			IE_KEY_F18 = 105,
			IE_KEY_F19 = 106,
			IE_KEY_F20 = 107,
			IE_KEY_F21 = 108,
			IE_KEY_F22 = 109,
			IE_KEY_F23 = 110,
			IE_KEY_F24 = 111,
			IE_KEY_F25 = 112,

			// === OTHER ===
			IE_KEY_INSERT = 113,
			IE_KEY_DELETE = 114,
			IE_KEY_PAGE_UP = 115,
			IE_KEY_PAGE_DOWN = 116,
			IE_KEY_HOME = 117,
			IE_KEY_END = 118,
			IE_KEY_SCROLLLOCK = 119,
			IE_KEY_NUMLOCK = 120,
			IE_KEY_PRINTSCREEN = 121,
			IE_KEY_PAUSE = 122,
			IE_KEY_MENU = 123,
			IE_KEY_WORLD_1 = 124,
			IE_KEY_WORLD_2 = 125,
			IE_KEY_LEFT_SUPER = 126,
			IE_KEY_RIGHT_SUPER = 127
		};
	}
}