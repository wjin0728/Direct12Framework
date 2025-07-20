#include "stdafx.h"
#include "Button.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"


void CButton::Awake()
{
}

void CButton::Start()
{

}

void CButton::Update()
{
	Vec2 mousePos = INPUT.GetMousePosition();
	//ndc space
	Vec2 screenSize = INSTANCE(CDX12Manager).GetRenderTargetSize();


	float screenX = ((mTargetPos.x + 1.0f) * 0.5f) * screenSize.x;
	float screenY = ((1.0f - mTargetPos.y) * 0.5f) * screenSize.y; 
	long width = int(mTargetSize.x * screenSize.x);	
	long height = int(mTargetSize.y * screenSize.y);

	SimpleMath::Rectangle mRect = SimpleMath::Rectangle(screenX, screenY, width, height);
	if (mRect.Contains(mousePos))
	{
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON))
		{
			mIsPressed = true;
		}
		else if (INPUT.IsKeyUp(KEY_TYPE::LBUTTON) && mIsPressed)
		{
			mIsPressed = false;
			if(mOnClick)
			{
				mOnClick();
			}
		}
		mIsHovered = true;
	}
	else {
		mIsPressed = false;
		mIsHovered = false;
	}

	
}

void CButton::LateUpdate()
{

}
