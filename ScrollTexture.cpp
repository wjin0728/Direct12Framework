#include "stdafx.h"
#include "ScrollTexture.h"
#include"InputManager.h"
#include"Timer.h"
#include"Transform.h"

CScrollTexture::CScrollTexture() : CMonoBehaviour("ScrollTexture")
{
}

CScrollTexture::~CScrollTexture()
{
}

void CScrollTexture::Awake()
{
}

void CScrollTexture::Start()
{
}

void CScrollTexture::Update()
{
	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		movement.y += DELTA_TIME*0.2f;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::S)) {
		movement.y -= DELTA_TIME*0.2f;
	}

	Matrix mat = Matrix::CreateTranslation(movement);
	GetTransform()->SetTexMat(mat);
	GetTransform()->dirtyFramesNum = 3;
}

void CScrollTexture::LateUpdate()
{
}
