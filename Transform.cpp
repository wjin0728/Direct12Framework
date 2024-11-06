#include "stdafx.h"
#include"DX12Manager.h"
#include"Transform.h"
#include"FrameResource.h"
#include"ObjectPoolManager.h"
#include"MeshRenderer.h"
#include"Collider.h"
#include"GameObject.h"

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
	copy->isMoved = isMoved;
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
	SetCBVIndex();
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
	isMoved = true;

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

	isMoved = true;
	dirtyFramesNum = FRAME_RESOURCE_COUNT;
}

void CTransform::SetCBVIndex()
{
	if (mCbvIdx < 0) {
		mCbvIdx = INSTANCE(CObjectPoolManager).GetTopCBVIndex();
	}
}

void CTransform::SetParent(std::shared_ptr<CTransform> parent)
{
	auto _parent = mParent.lock();
	auto& parentChildren = parent->GetOwner()->GetChildren();

	if (_parent) {
		auto it = std::find(parentChildren.begin(), parentChildren.end(), owner.lock());

		if (it != parentChildren.end()) {
			parentChildren.erase(it);
		}
	}

	mParent = parent;
	if (parent) {
		parentChildren.push_back(owner.lock());
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

void CTransform::BindConstantBuffer()
{
	CopyToCbvBuffer();
	UPLOADBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->UpdateBuffer(mCbvIdx);
}

void CTransform::MoveStrafe(float distance)
{
	mLocalPosition += (mLocalRight * distance);

	isMoved = true;
}

void CTransform::MoveUp(float distance)
{
	mLocalPosition += (mLocalUp * distance);

	isMoved = true;
}

void CTransform::MoveForward(float distance)
{
	mLocalPosition += (mLocalLook * distance);

	isMoved = true;
}

void CTransform::Move(const Vec3& direction, float distance)
{
	mLocalPosition += (direction * distance);

	isMoved = true;
}

void CTransform::LookTo(const Vec3& lookDir, const Vec3& up)
{
	Vec3 lookVec = lookDir.GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::LookAt(const Vec3& lookPos, const Vec3& up)
{
	Vec3 lookVec = (lookPos - mLocalPosition).GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::LookAt(const CTransform& target, const Vec3& up)
{
	Vec3 lookVec = (target.mLocalPosition - mLocalPosition).GetNormalized();
	Matrix rotateMat = Matrix::CreateWorld(Vec3::Zero, lookVec, up);
	mLocalRotation = Quaternion::CreateFromLocalRotationMatrix(rotateMat);

	isMoved = true;
}

void CTransform::Rotate(float pitch, float yaw, float roll)
{
	mLocalRotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) * mLocalRotation;

	isMoved = true;
}

void CTransform::Rotate(const Vec3& rotation)
{
	Vec3 angles = { XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y) ,XMConvertToRadians(rotation.z) };
	mLocalRotation = Quaternion::CreateFromYawPitchRoll(angles) * mLocalRotation;

	isMoved = true;
}

void CTransform::Rotate(const Vec3& axis, float angle)
{
	mLocalRotation = Quaternion::CreateFromAxisAngle(axis, angle) * mLocalRotation;

	isMoved = true;
}

const Matrix& CTransform::GetWorldMat()
{
	UpdateWorldMatrix();
	return mWorldMat;
}

void CTransform::UpdateLocalMatrix()
{
	mLocalMat = Matrix::CreateScale(mLocalScale) * Matrix::CreateFromQuaternion(mLocalRotation);
	mLocalMat._41 = mLocalPosition.x;
	mLocalMat._42 = mLocalPosition.y;
	mLocalMat._43 = mLocalPosition.z;
}

void CTransform::UpdateWorldMatrix()
{
	auto parent = mParent.lock();

	if (!isMoved && !parent) {
		return;
	}

	if (isMoved) {
		UpdateLocalMatrix();
		isMoved = false;
	}

	mWorldMat = parent ? (mLocalMat * parent->GetWorldMat()) : mLocalMat;

	dirtyFramesNum = FRAME_RESOURCE_COUNT;

	auto collider = GetOwner()->GetCollider();
	if (collider) {
		collider->UpdateOOBB(mWorldMat);
	}
}


void CTransform::CopyToCbvBuffer()
{
	UpdateWorldMatrix();

	if (dirtyFramesNum <= 0) {
		return;
	}

	CBObjectData objDate;
	objDate.worldMAt = mWorldMat.Transpose();
	objDate.textureMat = mTextureMat.Transpose();

	auto meshRenderer = owner.lock()->GetMeshRendere();
	if (meshRenderer)
		objDate.materialIdx = meshRenderer->GetMaterialIndex();

	UPLOADBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->CopyData(&objDate, mCbvIdx);

	dirtyFramesNum--;
}

