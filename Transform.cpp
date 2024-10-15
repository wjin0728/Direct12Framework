#include "stdafx.h"
#include "Transform.h"
#include"FrameResource.h"
#include"GameObject.h"

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
	auto parent = mParent.lock();
	bool parentMoved = false;

	if (parent) parentMoved = parent->isMoved;

	if (isMoved || parentMoved)
		dirtyFramesNum = FRAME_RESOURCE_COUNT;

	if (dirtyFramesNum > 0) {
		if (isMoved) {
			mLocalMat = Matrix::CreateScale(mScale) * Matrix::CreateFromQuaternion(mRotation);
			mLocalMat._41 = mPosition.x; mLocalMat._42 = mPosition.y; mLocalMat._43 = mPosition.z;

			mWorldMat = mLocalMat;
			if (parent) mWorldMat *= parent->mWorldMat;
		}
		else if (parentMoved) {
			mWorldMat = mLocalMat * parent->mWorldMat;
		}
		dirtyFramesNum--;
	}
}

void CTransform::FixedUpdate()
{
}

void CTransform::SetParent(std::shared_ptr<CTransform> parent)
{
	auto _parent = mParent.lock();

	if (_parent) {
		auto it = std::find(_parent->mChildren.begin(), _parent->mChildren.end(), this);

		if (it != _parent->mChildren.end()) {
			_parent->mChildren.erase(it);
		}
	}

	mParent = parent;
	if (parent) {
		parent->mChildren.push_back(std::shared_ptr<CTransform>(this));
	}
}

void CTransform::MoveStrafe(float distance)
{
	mPosition += (mRight * distance);

	isMoved = true;
}

void CTransform::MoveUp(float distance)
{
	mPosition += (mUp * distance);

	isMoved = true;
}

void CTransform::MoveForward(float distance)
{
	mPosition += (mLook * distance);

	isMoved = true;
}

void CTransform::Move(const Vec3& direction, float distance)
{
	mPosition += (direction * distance);

	isMoved = true;
}

void CTransform::LookTo(const Vec3& lookDir, const Vec3& up)
{
	Vec3 lookVec = lookDir.GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(mPosition, lookVec, up);
	mRotation = Quaternion::CreateFromRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::LookAt(const Vec3& lookPos, const Vec3& up)
{
	Matrix rotateMat = Matrix::CreateLookAt(mPosition, lookPos, up);
	mRotation = Quaternion::CreateFromRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::LookAt(const CTransform& target, const Vec3& up)
{
	Matrix rotateMat = Matrix::CreateLookAt(mPosition, target.mPosition, up);
	mRotation = Quaternion::CreateFromRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
	mRotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll) * mRotation;

	isMoved = true;
}

void CTransform::Rotate(const Vec3& rotation)
{
	mRotation = Quaternion::CreateFromYawPitchRoll(rotation) * mRotation;

	isMoved = true;
}

void CTransform::Rotate(const Vec3& axis, float angle)
{
	mRotation = Quaternion::CreateFromAxisAngle(axis, angle) * mRotation;

	isMoved = true;
}

std::wstring CTransform::GetObjectName()
{
	if(!owner.expired()) 
		return owner.lock()->GetName();

	return L"Unknown";
}

std::shared_ptr<CTransform> CTransform::FindChild(const std::wstring& name)
{
	for (const auto& child : mChildren) {
		if (name == child->GetObjectName())
			return child;
	}
	return nullptr;
}

