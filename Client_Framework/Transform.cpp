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
	copy->dirtyFramesNum = FRAME_RESOURCE_COUNT;

	return copy;
}

void CTransform::Awake()
{
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
	mDirtyFlag = true;

	UpdateWorldMatrix();
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
		auto scene = INSTANCE(CSceneManager).GetCurScene();
		if (scene) {
			scene->RemoveObject(ownerObj);
		}
		else {
			ownerObj->SetActive(false);
		}
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
		auto scene = INSTANCE(CSceneManager).GetCurScene();
		if (scene) {
			scene->AddObject(GetRoot()->owner->GetRenderLayer(), ownerObj);
		}
		else {
			ownerObj->SetActive(false);
		}

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

	mLocalRotation = Quaternion::LookRotation(lookVec);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation) * radToDeg;

	mDirtyFlag = true;
}

void CTransform::LookAt(const Vec3& lookPos, const Vec3& up)
{
	Vec3 lookVec = (lookPos - mLocalPosition).GetNormalized();
	mLocalRotation = Quaternion::LookRotation(lookVec);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation) * radToDeg;

	mDirtyFlag = true;
}

void CTransform::LookAt(const CTransform& target, const Vec3& up)
{
	Vec3 lookVec = (target.mLocalPosition - mLocalPosition).GetNormalized();
	mLocalRotation = Quaternion::LookRotation(lookVec);
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation) * radToDeg;

	mDirtyFlag = true;
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
	mLocalRotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		yaw * degToRad, pitch * degToRad, roll * degToRad) * mLocalRotation;
	mLocalEulerAngle.x += pitch; mLocalEulerAngle.y += yaw; mLocalEulerAngle.z += roll;

	mDirtyFlag = true;
}

void CTransform::Rotate(const Vec3& rotation)
{
	Vec3 angles = { rotation.x * degToRad, rotation.y * degToRad ,rotation.z * degToRad };
	mLocalRotation = Quaternion::CreateFromYawPitchRoll(angles) * mLocalRotation;
	mLocalEulerAngle += rotation;

	mDirtyFlag = true;
}

void CTransform::Rotate(const Vec3& axis, float angle)
{
	mLocalRotation = Quaternion::CreateFromAxisAngle(axis, angle * degToRad) * mLocalRotation;
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation) * radToDeg;

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

void CTransform::RotateAround(const Vec3& point, const Vec3& axis, float angle)
{
	Vec3 normalizedAxis = axis.GetNormalized();

	Quaternion rot = Quaternion::CreateFromAxisAngle(normalizedAxis, angle);
	Vec3 dir = mLocalPosition - point;
	Quaternion dirQuat(dir.x, dir.y, dir.z, 0.0f);
	Quaternion rotatedQuat = rot * dirQuat * rot.Inverse();
	Vec3 rotatedDir(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
	mLocalPosition = point + rotatedDir;
	mLocalEulerAngle = Vec3::GetAngleToQuaternion(mLocalRotation) * radToDeg;
	mDirtyFlag = true;
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

	mLocalRight = Vec3(mLocalMat._11, mLocalMat._12, mLocalMat._13).GetNormalized();
	mLocalUp = Vec3(mLocalMat._21, mLocalMat._22, mLocalMat._23).GetNormalized();
	mLocalLook = Vec3(mLocalMat._31, mLocalMat._32, mLocalMat._33).GetNormalized();
}

void CTransform::UpdateWorldMatrix(bool update)
{
	std::shared_ptr<CTransform> parent = mParent.lock();

	if (!mDirtyFlag && !parent) {
		return;
	}

	if (mDirtyFlag) {                       
		UpdateLocalMatrix();
		mDirtyFlag = false;
	}

	mWorldMat = parent ? (mLocalMat * parent->GetWorldMat(update)) : mLocalMat;

	dirtyFramesNum = FRAME_RESOURCE_COUNT;

	owner->mRootLocalBS.Transform(owner->mRootBS, mWorldMat);
}

void CTransform::UpdateWorldMatrix(std::shared_ptr<CTransform> parent, bool update)
{
	if (!mDirtyFlag && !parent) {
		return;
	}

	if (mDirtyFlag) {
		UpdateLocalMatrix();
		mDirtyFlag = false;
	}

	mWorldMat = parent ? (mLocalMat * parent->GetWorldMat(update)) : mLocalMat;

	dirtyFramesNum = FRAME_RESOURCE_COUNT;

	owner->mRootLocalBS.Transform(owner->mRootBS, mWorldMat);
}

void CTransform::ApplyBlendedTransform()
{
	SetLocalScale(mScaleLayerBlending);
	SetLocalRotation(mRotationLayerBlending);
	SetLocalPosition(mPositionLayerBlending);
	mDirtyFlag = true;
}

void CTransform::BlendingTransform(const ANIMATION_BLEND_TYPE blendType, const Vec3& scale, const Vec3& rotation, const Vec3& position, float weight)
{
	switch (blendType) {
	case ANIMATION_BLEND_TYPE::ADDITIVE: {
		mScaleLayerBlending += scale;
		mRotationLayerBlending += rotation;
		mPositionLayerBlending += position;
		break;
	}
	case ANIMATION_BLEND_TYPE::OVERRIDE: {
		mScaleLayerBlending = scale;
		mRotationLayerBlending = rotation;
		mPositionLayerBlending = position;
		break;
	}
	case ANIMATION_BLEND_TYPE::OVERRIDE_PASSTHROUGH: {
		mScaleLayerBlending += scale * weight;
		mRotationLayerBlending += rotation * weight;
		mPositionLayerBlending += position * weight;
		break;
	}
	}
}

void CTransform::PrintSRT()
{
	std::cout << mLocalScale.x << " " << mLocalScale.y << " " << mLocalScale.z << std::endl;
	std::cout << mLocalEulerAngle.x << " " << mLocalEulerAngle.y << " " << mLocalEulerAngle.z << std::endl;
	std::cout << mLocalPosition.x << " " << mLocalPosition.y << " " << mLocalPosition.z << std::endl;
	std::cout << mLocalRotation.x << " " << mLocalRotation.y << " " << mLocalRotation.z << " " << mLocalRotation.w << std::endl;
	std::cout << std::endl;
	PrintMatrix(mLocalMat);
	std::cout << "======================" << std::endl;
}