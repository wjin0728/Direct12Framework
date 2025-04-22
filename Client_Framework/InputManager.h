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

