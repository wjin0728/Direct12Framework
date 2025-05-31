#pragma once
#include"stdafx.h"

#define INPUT INSTANCE(CInputManager)

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,
	SHIFT = VK_SHIFT,
	CTRL = VK_CONTROL,
	ESCAPE = VK_ESCAPE,
	SPACE = VK_SPACE,
	ENTER = VK_RETURN,
	ALT = VK_MENU,

	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',
	F = 'F',
	L = 'L',
	P = 'P',

	ONE = '1',
	TWO = '2',
	THREE = '3',
	FOUR = '4',
	FIVE = '5',
	SIX = '6',
	SEVEN = '7',
	EIGHT = '8',
	NINE = '9',

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F4 = VK_F4,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<UINT>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<UINT>(KEY_STATE::END),
};

class CInputManager
{
	MAKE_SINGLETON(CInputManager)

private:
	HWND mHwnd{};
	std::array<KEY_STATE, KEY_TYPE_COUNT> mStates{};

	POINT mPrevMousePos{};
	Vec2 mMouseDelta{};
	bool mFixMousePos{};

public:
	void Initialize(HWND hwnd);
	void Update();

	bool IsKeyPress(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	bool IsKeyDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	bool IsKeyUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	void FixMousePosition(bool fix);
	void ChangeMouseState();
	Vec2 GetMouseDelta() const { return mMouseDelta; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return mStates[static_cast<UINT>(key)]; }
	void MoveMouseToCenter();
};

