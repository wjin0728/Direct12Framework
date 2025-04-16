#include "stdafx.h"
#include "InputManager.h"

void CInputManager::Initialize(HWND hwnd)
{
	mHwnd = hwnd;
	mStates.fill(KEY_STATE::NONE);
	FixMousePosition(true);
}

void CInputManager::Update()
{
	HWND hwnd = ::GetActiveWindow();
	if (mHwnd != hwnd)
	{
		mStates.fill(KEY_STATE::NONE);
		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT]{};
	if (::GetKeyboardState(asciiKeys) == false) {
		return;
	}

	for (UINT16 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		KEY_STATE& state = mStates[key];
		if (asciiKeys[key] & 0x80)
		{
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN) {
				state = KEY_STATE::PRESS;
			}
			else {
				state = KEY_STATE::DOWN;
			}
		}
		else
		{
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN) {
				state = KEY_STATE::UP;
			}
			else {
				state = KEY_STATE::NONE;
			}
		}
	}


	if (GetCapture() == mHwnd) {
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		mMouseDelta.x = (float)(ptCursorPos.x - mPrevMousePos.x) / 3.0f;
		mMouseDelta.y = (float)(ptCursorPos.y - mPrevMousePos.y) / 3.0f;
		SetCursorPos(mPrevMousePos.x, mPrevMousePos.y);

		if(!mFixMousePos) mPrevMousePos = ptCursorPos;
	}
}

void CInputManager::FixMousePosition(bool fix)
{
	mFixMousePos = fix;

	if (fix) {
		SetCapture(mHwnd);
		MoveMouseToCenter();
	}
	else {
		ReleaseCapture();
	}
}

void CInputManager::ChangeMouseState()
{
	FixMousePosition(!mFixMousePos);
}

void CInputManager::MoveMouseToCenter()
{
	RECT rect;
	GetClientRect(mHwnd, &rect);

	int centerX = rect.left + (rect.right - rect.left) / 2;
	int centerY = rect.top + (rect.bottom - rect.top) / 2;

	POINT centerPoint = { centerX, centerY };
	ClientToScreen(mHwnd, &centerPoint);
	mPrevMousePos = centerPoint;
	SetCursorPos(mPrevMousePos.x, mPrevMousePos.y);
}
