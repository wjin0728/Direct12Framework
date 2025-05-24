#include "stdafx.h"
#include "Collider.h"
#include "RigidBody.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"Timer.h"

CCollider::CCollider()
{
}

CCollider::~CCollider()
{
}

std::shared_ptr<CComponent> CCollider::Clone()
{
	std::shared_ptr<CCollider> copy = std::make_shared<CCollider>();
	copy->mLocalOobb = mLocalOobb;
	copy->mWorldOOBB = mWorldOOBB;

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
	UpdateOOBB(GetTransform()->GetWorldMat());
}

void CCollider::UpdateOOBB(const Matrix& worldMat)
{
	mLocalOobb.Transform(mWorldOOBB, worldMat);
}

void CCollider::Initialize(const Vec3& center, const Vec3& size)
{
	mLocalOobb = BoundingOrientedBox(center, size, { 0.f,0.f,0.f,1.f });
}

void CCollider::Initialize(const BoundingOrientedBox& oobb)
{
	mLocalOobb = oobb;
}
