#include "stdafx.h"
#include "Transform.h"
#include"FrameResource.h"

CTransform::CTransform() : CComponent(COMPONENT_TYPE::TRANSFORM), dirtyFramesNum(FRAME_RESOURCE_COUNT)
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
	mPosition = mPosition + (mRight * distance);
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::MoveUp(float distance)
{
	mPosition = mPosition + (mUp * distance);
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::MoveForward(float distance)
{
	mPosition = mPosition + (mLook * distance);
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::Move(const XMFLOAT3& direction, float distance)
{
	mPosition = mPosition + (direction * distance);
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::LookTo(const XMFLOAT3& lookDir, const XMFLOAT3& up)
{
	XMFLOAT3 lookVec = Vector3::Normalize(lookDir);
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::LookTo(const CTransform& target, const XMFLOAT3& up)
{
	XMFLOAT3 targetDir = target.mPosition - mPosition;
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::LookAt(const XMFLOAT3& lookPos, const XMFLOAT3& up)
{

	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::LookAt(const CTransform& target, const XMFLOAT3& up)
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::Rotate(const XMFLOAT3& rotation)
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::Rotate(const XMFLOAT3& axis, float angle)
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}
