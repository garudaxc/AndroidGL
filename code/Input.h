#pragma once

#include <vector>
#include "Platfrom.h"

using namespace std;

namespace FancyTech
{
		
	class Event
	{
	public:
		enum EventType
		{
			MouseMove = 0,
			LButtonDown,
			LButtonUp,
			MButtonDown,
			MButtonUp,
			RButtonDown,
			RButtonUp,
			MouseWheel,

			KeyDown,
			KeyUp,
		};

		enum Parameter
		{
			PM_LButton = 0x0001,
			PM_RButton = 0x0002,
			PM_Shift = 0x0004,
			PM_Control = 0x0008,
			PM_MButton = 0x0010,
		};


		enum KeyCode
		{
			KEYCODE_BACK = 0x08,
			KEYCODE_TAB = 0x09,

			KEYCODE_CLEAR = 0x0C,
			KEYCODE_RETURN = 0x0D,

			KEYCODE_SHIFT = 0x10,
			KEYCODE_CONTROL = 0x11,
			KEYCODE_MENU = 0x12,
			KEYCODE_PAUSE = 0x13,
			KEYCODE_CAPITAL = 0x14,

			KEYCODE_ESCAPE = 0x1B,

			KEYCODE_CONVERT = 0x1C,
			KEYCODE_NONCONVERT = 0x1D,
			KEYCODE_ACCEPT = 0x1E,
			KEYCODE_MODECHANGE = 0x1F,

			KEYCODE_SPACE = 0x20,
			KEYCODE_PRIOR = 0x21,
			KEYCODE_NEXT = 0x22,
			KEYCODE_END = 0x23,
			KEYCODE_HOME = 0x24,
			KEYCODE_LEFT = 0x25,
			KEYCODE_UP = 0x26,
			KEYCODE_RIGHT = 0x27,
			KEYCODE_DOWN = 0x28,
			KEYCODE_SELECT = 0x29,
			KEYCODE_PRINT = 0x2A,
			KEYCODE_EXECUTE = 0x2B,
			KEYCODE_SNAPSHOT = 0x2C,
			KEYCODE_INSERT = 0x2D,
			KEYCODE_DELETE = 0x2E,
			KEYCODE_HELP = 0x2F,

			KEYCODE_0 = 0x30,
			KEYCODE_1 = 0x31,
			KEYCODE_2 = 0x32,
			KEYCODE_3 = 0x33,
			KEYCODE_4 = 0x34,
			KEYCODE_5 = 0x35,
			KEYCODE_6 = 0x36,
			KEYCODE_7 = 0x37,
			KEYCODE_8 = 0x38,
			KEYCODE_9 = 0x39,

			KEYCODE_A = 0x41,
			KEYCODE_B = 0x42,
			KEYCODE_C = 0x43,
			KEYCODE_D = 0x44,
			KEYCODE_E = 0x45,
			KEYCODE_F = 0x46,
			KEYCODE_G = 0x47,
			KEYCODE_H = 0x48,
			KEYCODE_I = 0x49,
			KEYCODE_J = 0x4A,
			KEYCODE_K = 0x4B,
			KEYCODE_L = 0x4C,
			KEYCODE_M = 0x4D,
			KEYCODE_N = 0x4E,
			KEYCODE_O = 0x4F,
			KEYCODE_P = 0x50,
			KEYCODE_Q = 0x51,
			KEYCODE_R = 0x52,
			KEYCODE_S = 0x53,
			KEYCODE_T = 0x54,
			KEYCODE_U = 0x55,
			KEYCODE_V = 0x56,
			KEYCODE_W = 0x57,
			KEYCODE_X = 0x58,
			KEYCODE_Y = 0x59,
			KEYCODE_Z = 0x5A,

			/*
			* KEYCODE_0 - KEYCODE_9 are the same as ASCII '0' - '9' (= 0x30 - = 0x39)
			* = 0x40 : unassigned
			* KEYCODE_A - KEYCODE_Z are the same as ASCII 'A' - 'Z' (= 0x41 - = 0x5A)
			*/

			KEYCODE_LWIN = 0x5B,
			KEYCODE_RWIN = 0x5C,
			KEYCODE_APPS = 0x5D,

			/*
			* = 0x5E : reserved
			*/

			KEYCODE_SLEEP = 0x5F,

			KEYCODE_NUMPAD0 = 0x60,
			KEYCODE_NUMPAD1 = 0x61,
			KEYCODE_NUMPAD2 = 0x62,
			KEYCODE_NUMPAD3 = 0x63,
			KEYCODE_NUMPAD4 = 0x64,
			KEYCODE_NUMPAD5 = 0x65,
			KEYCODE_NUMPAD6 = 0x66,
			KEYCODE_NUMPAD7 = 0x67,
			KEYCODE_NUMPAD8 = 0x68,
			KEYCODE_NUMPAD9 = 0x69,
			KEYCODE_MULTIPLY = 0x6A,
			KEYCODE_ADD = 0x6B,
			KEYCODE_SEPARATOR = 0x6C,
			KEYCODE_SUBTRACT = 0x6D,
			KEYCODE_DECIMAL = 0x6E,
			KEYCODE_DIVIDE = 0x6F,
			KEYCODE_F1 = 0x70,
			KEYCODE_F2 = 0x71,
			KEYCODE_F3 = 0x72,
			KEYCODE_F4 = 0x73,
			KEYCODE_F5 = 0x74,
			KEYCODE_F6 = 0x75,
			KEYCODE_F7 = 0x76,
			KEYCODE_F8 = 0x77,
			KEYCODE_F9 = 0x78,
			KEYCODE_F10 = 0x79,
			KEYCODE_F11 = 0x7A,
			KEYCODE_F12 = 0x7B,

			/*
			* = 0x88 - = 0x8F : unassigned
			*/

			KEYCODE_NUMLOCK = 0x90,
			KEYCODE_SCROLL = 0x91,
			KEYCODE_MAX

		};


		uint16_t	Type;
		uint16_t	Param;

		int			xPos;
		int			yPos;

		union
		{
			int		zDelta;
			int		Code;
		};
	};


	class EventReceiver
	{
	public:
		EventReceiver* InitNext;
		EventReceiver();
		~EventReceiver();

		virtual bool	OnEvent(const Event& event) = 0;

	private:
	};



	class Input
	{
	public:
		Input();
		~Input();

		void	Register(EventReceiver* receiver);
		void	Dispatch(const Event& event);

	protected:
		vector<EventReceiver*>	receivers_;
	};
	

	extern Input* GInput;


}