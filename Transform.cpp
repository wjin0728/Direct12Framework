#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform() : CComponent(COMPONENT_TYPE::TRANSFORM)
{
}

CTransform::~CTransform()
{
}

void CTransform::Awake()
{
}

void CTransform::Start()
{
}

void CTransform::Update()
{
}

void CTransform::LateUpdate()
{
}

void CTransform::FixedUpdate()
{
}

void CTransform::MoveStrafe(float distance)
{
	mLocalMat._41 += (mRight.x * distance);
	mLocalMat._42 += (mRight.y * distance);
	mLocalMat._43 += (mRight.z * distance);
}

void CTransform::MoveUp(float distance)
{
}

void CTransform::MoveForward(float distance)
{
}

void CTransform::Move(const XMFLOAT3& direction, float speed)
{
}

void CTransform::LookTo(const XMFLOAT3& lookDir, const XMFLOAT3& up)
{
}

void CTransform::LookAt(const XMFLOAT3& lookPos, const XMFLOAT3& up)
{
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
}

void CTransform::Rotate(const XMFLOAT3& rotation)
{
}

void CTransform::Rotate(const XMFLOAT3& axis, float angle)
{
}
