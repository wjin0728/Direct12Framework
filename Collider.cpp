#include "stdafx.h"
#include "Collider.h"
#include "RigidBody.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Timer.h"

CCollider::CCollider() : CComponent(COMPONENT_TYPE::COLLIDER)
{
}

CCollider::~CCollider()
{
}

std::shared_ptr<CComponent> CCollider::Clone()
{
	std::shared_ptr<CCollider> copy = std::make_shared<CCollider>();
	copy->mLocalAabb = mLocalAabb;
	copy->mLocalOobb = mLocalOobb;
	copy->mWorldOOBB = mWorldOOBB;
	copy->mScale = mScale;

	return copy;
}

void CCollider::Awake()
{
	
}

void CCollider::Start()
{
	UpdateOOBB(Matrix::Identity);
}

void CCollider::Update()
{
}

void CCollider::LateUpdate()
{
}

void CCollider::UpdateOOBB(const Matrix& worldMat)
{
	Matrix mat = Matrix::CreateScale(mScale) * worldMat;
	mLocalOobb.Transform(mWorldOOBB, mat);
}

void CCollider::SetLocalOOBB(const BoundingOrientedBox& oobb)
{
	mLocalOobb = oobb;
	mLocalAabb = BoundingBox::BoundingBox(mLocalOobb.Center, mLocalOobb.Extents);
}
