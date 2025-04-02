#include "stdafx.h"
#include"DX12Manager.h"
#include"Transform.h"
#include"FrameResource.h"
#include"ObjectPoolManager.h"
#include"MeshRenderer.h"
#include"Collider.h"
#include"GameObject.h"
#include"SceneManager.h"
#include"Scene.h"

CTransform::CTransform() : CComponent(COMPONENT_TYPE::TRANSFORM), dirtyFramesNum(FRAME_RESOURCE_COUNT)
{
}

CTransform::~CTransform()
{
	ReturnCBVIndex();
}

std::shared_ptr<CComponent> CTransform::Clone()
{
	std::shared_ptr<CTransform> copy = std::make_shared<CTransform>();
	copy->mDirtyFlag = mDirtyFlag;
	copy->mLocalRight = mLocalRight;
	copy->mLocalUp = mLocalUp;
	copy->mLocalLook = mLocalLook;
	copy->mLocalPosition = mLocalPosition;
	copy->mLocalScale = mLocalScale;
	copy->mLocalRotation = mLocalRotation;
	copy->mWorldMat = mWorldMat;
	copy->mLocalMat = mLocalMat;
	copy->dirtyFramesNum = dirtyFramesNum;

	return copy;
}

void CTransform::Awake()
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
	mDirtyFlag = true;

	if (owner->mTag == L"Billboard") {
		std::cout << "";
	}

	UpdateWorldMatrix();

	if(!owner->mIsInstancing) SetCBVIndex();
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

void CTransform::Reset()
{
	mLocalRight = Vec3::Right;;
	mLocalUp = Vec3::Up;
	mLocalLook = Vec3::Backward;

	mLocalPosition = Vec3::Zero;
	mLocalScale = { 1.f,1.f,1.f };
	mLocalRotation = Quaternion::Identity;

	mWorldMat = Matrix::Identity;
	mLocalMat = Matrix::Identity;

	mDirtyFlag = true;
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::SetCBVIndex()
{
	if (mCbvIdx < 0) {
		mCbvIdx = INSTANCE(CObjectPoolManager).GetTopCBVIndex();
	}
}

void CTransform::SetParent(std::shared_ptr<CTransform> parent, bool isKeepLocalMat)
{
	auto curParent = mParent.lock();
	auto ownerObj = owner->GetSptrFromThis();

	if (curParent) {
		if (curParent.get() == parent.get()) {
			return;
		}
		curParent->GetOwner()->RemoveChild(ownerObj);
	}
	else {
		if (!parent) {
			return;
		}
		INSTANCE(CSceneManager).GetCurScene()->RemoveObject(ownerObj);
	}
	
	UpdateWorldMatrix();

	mParent = parent;

	if (parent) {
		parent->GetOwner()->AddChild(ownerObj);

		if (isKeepLocalMat) {
			mLocalMat = mWorldMat * parent->GetWorldMat().Invert();
			mDirtyFlag = true;
		}
	}
	else {
		INSTANCE(CSceneManager).GetCurScene()->AddObject(GetRoot()->owner->GetRenderLayer(), ownerObj);

		if (isKeepLocalMat) {
			mLocalMat = mWorldMat;
			mDirtyFlag = true;
		}
	}
}

Vec3 CTransform::GetWorldPosition()
{
	UpdateWorldMatrix();
	return Vec3(mWorldMat._41, mWorldMat._42, mWorldMat._43);
}

void CTransform::ReturnCBVIndex()
{
	if (mCbvIdx < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnCBVIndex(mCbvIdx);
	mCbvIdx = -1;
}

void CTransform::MoveStrafe(float distance)
{
	mLocalPosition += (mLocalRight * distance);

	mDirtyFlag = true;
}

void CTransform::MoveUp(float distance)
{
	mLocalPosition += (mLocalUp * distance);

	mDirtyFlag = true;
}

void CTransform::MoveForward(float distance)
{
	mLocalPosition += (mLocalLook * distance);

	mDirtyFlag = true;
}

void CTransform::Move(const Vec3& direction, float distance)
{
	mLocalPosition += (direction * distance);

	mDirtyFlag = true;
}

void CTransform::LookTo(const Vec3& lookDir, const Vec3& up)
{
	Vec3 lookVec = lookDir.GetNormalized();

	Vec3 Up = up.GetNormalized();
	if (lookDir == Vec3(0.f, -1.f, 0.f) || lookDir == Vec3(0.f, 1.f, 0.f)) Up = Vec3(0.f, 0.f, 1.f);

	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, Up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation);

	mDirtyFlag = true;
}

void CTransform::LookAt(const Vec3& lookPos, const Vec3& up)
{
	Vec3 lookVec = (lookPos - mLocalPosition).GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation);

	mDirtyFlag = true;
}

void CTransform::LookAt(const CTransform& target, const Vec3& up)
{
	Vec3 lookVec = (target.mLocalPosition - mLocalPosition).GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation);

	mDirtyFlag = true;
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
	mLocalRotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(yaw), XMConvertToRadians(pitch), XMConvertToRadians(roll)) * mLocalRotation;
	mLocalEulerAngle.x += pitch; mLocalEulerAngle.y += yaw; mLocalEulerAngle.z += roll;

	mDirtyFlag = true;
}

void CTransform::Rotate(const Vec3& rotation)
{
	Vec3 angles = { XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y) ,XMConvertToRadians(rotation.z) };
	mLocalRotation = Quaternion::CreateFromYawPitchRoll(angles) * mLocalRotation;
	mLocalEulerAngle += rotation;

	mDirtyFlag = true;
}

void CTransform::Rotate(const Vec3& axis, float angle)
{
	mLocalRotation = Quaternion::CreateFromAxisAngle(axis, angle) * mLocalRotation;
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation);

	mDirtyFlag = true;
}

void CTransform::RotateX(float angle)
{
	Rotate(angle, 0.f, 0.f);
}

void CTransform::RotateY(float angle)
{
	Rotate(0.f, angle, 0.f);
}

void CTransform::RotateZ(float angle)
{
	Rotate(0.f, 0.f, angle);
}

const Matrix& CTransform::GetWorldMat(bool update)
{
	if(update && !owner->mIsStatic) UpdateWorldMatrix();
	return mWorldMat;
}

std::shared_ptr<CTransform> CTransform::GetRoot()
{
	auto parent = mParent.lock();
	if (!parent) {
		return shared_from_this();
	}
	return parent->GetRoot();
}

void CTransform::UpdateLocalMatrix()
{
	mLocalMat = Matrix::CreateScale(mLocalScale) * Matrix::CreateFromQuaternion(mLocalRotation);
	mLocalMat._41 = mLocalPosition.x;
	mLocalMat._42 = mLocalPosition.y;
	mLocalMat._43 = mLocalPosition.z;

	mLocalRight = Vec3(mLocalMat._11, mLocalMat._12, mLocalMat._13);
	mLocalUp = Vec3(mLocalMat._21, mLocalMat._22, mLocalMat._23);
	mLocalLook = Vec3(mLocalMat._31, mLocalMat._32, mLocalMat._33);
}

void CTransform::UpdateWorldMatrix()
{
	auto parent = mParent.lock();

	if (!mDirtyFlag && !parent) {
		return;
	}

	if (mDirtyFlag) {                       
		UpdateLocalMatrix();
		mDirtyFlag = false;
	}

	mWorldMat = parent ? (mLocalMat * parent->GetWorldMat()) : mLocalMat;

	dirtyFramesNum = FRAME_RESOURCE_COUNT;

	auto collider = GetOwner()->GetCollider();
	auto meshRenderer = GetOwner()->GetMeshRenderer();

	GetOwner()->mRootLocalBS.Transform(GetOwner()->mRootBS, mWorldMat);
	if (collider) {
		collider->UpdateOOBB(mWorldMat);
	}
	
}

void CTransform::ApplyBlendedTransform()
{
	mLocalScale = mScaleLayerBlending;
	mLocalEulerAngle = mRotationLayerBlending;
	mLocalPosition = mPositionLayerBlending;
}