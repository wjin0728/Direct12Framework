#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include"GameObject.h"
#include"Transform.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"UploadBuffer.h"
#include"GameObject.h"
#include"MeshRenderer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

std::shared_ptr<CComponent> CCamera::Clone()
{
	std::shared_ptr<CCamera> copy = std::make_shared<CCamera>();

	return copy;
}

void CCamera::GenerateViewMatrix()
{
	auto transform = GetTransform(); 
	const Matrix& worldMat = transform->GetWorldMat();

	Vec3 position = Vec3(worldMat._41, worldMat._42, worldMat._43);
	Vec3 right = worldMat.Right();
	Vec3 up = worldMat.Up();
	Vec3 look = worldMat.Backward();

	mViewMat._11 = right.x; mViewMat._12 = up.x; mViewMat._13 = look.x;
	mViewMat._21 = right.y; mViewMat._22 = up.y; mViewMat._23 = look.y;
	mViewMat._31 = right.z; mViewMat._32 = up.z; mViewMat._33 = look.z;
	mViewMat._41 = -(position.Dot(right));
	mViewMat._42 = -(position.Dot(up));
	mViewMat._43 = -(position.Dot(look));

	mViewPerspectiveProjectMat = mViewMat * mPerspectiveProjectMat;
	mViewOrthographicProjectMat = mViewMat * mOrthographicProjectMat;

	mInverseViewMat._11 = right.x; mInverseViewMat._12 = right.y; mInverseViewMat._13 = right.z;
	mInverseViewMat._21 = up.x; mInverseViewMat._22 = up.y; mInverseViewMat._23 = up.z;
	mInverseViewMat._31 = look.x; mInverseViewMat._32 = look.y; mInverseViewMat._33 = look.z;
	mInverseViewMat._41 = position.x; mInverseViewMat._42 = position.y; mInverseViewMat._43 = position.z;

	mFrustumView.Transform(mFrustumWorld, mInverseViewMat);

	//mFrustumShadow는 조금 더 크게 설정
	Matrix shadowMat = Matrix::CreateScale({1.5f, 1.2f, 1.f}) * mInverseViewMat;
	mFrustumView.Transform(mFrustumShadow, shadowMat);
}

void CCamera::Awake()
{
	GenerateViewMatrix();
}

void CCamera::Start()
{
}

void CCamera::Update()
{
}

void CCamera::LateUpdate()
{
	GenerateViewMatrix();
}

void CCamera::SetFOVAngle(float fovAngle)
{
	mFovAngle = fovAngle;
	mProjectRectDist = float(1.0f / tan(XMConvertToRadians(mFovAngle * 0.5f)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float nearPlane, float farPlane, float fovAngle)
{
	mAspectRatio = (float(mViewport.Width) / float(mViewport.Height));

	mPerspectiveProjectMat = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fovAngle), mAspectRatio, nearPlane, farPlane);

	mNearZ = nearPlane;
	mFarZ = farPlane;
	SetFOVAngle(fovAngle);

	BoundingFrustum::CreateFromMatrix(mFrustumView, mPerspectiveProjectMat);
}

void CCamera::GenerateReverseZPerspectiveProjectionMatrix(float nearPlane, float farPlane, float fovAngle)
{
	Matrix reverse_z = { 1.0f, 0.0f,  0.0f, 0.0f,
							0.0f, 1.0f,  0.0f, 0.0f,
							0.0f, 0.0f, -1.0f, 0.0f,
							0.0f, 0.0f,  1.0f, 1.0f };

	mAspectRatio = (float(mViewport.Width) / float(mViewport.Height));
	mNearZ = nearPlane;
	mFarZ = farPlane;
	Matrix commonPerspectiveMat = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fovAngle), mAspectRatio, nearPlane, farPlane);
	BoundingFrustum::CreateFromMatrix(mFrustumView, commonPerspectiveMat);
	SetFOVAngle(fovAngle);

	mPerspectiveProjectMat = commonPerspectiveMat * reverse_z;
}

void CCamera::GenerateOrthographicProjectionMatrix(float nearPlane, float farPlane, float width, float height)
{
	mOrthographicProjectMat = Matrix::CreateOrthographic(width, height, nearPlane, farPlane);

	mNearZ = nearPlane;
	mFarZ = farPlane;

	BoundingFrustum::CreateFromMatrix(mFrustumView, mOrthographicProjectMat);
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	mViewport.TopLeftX = float(xTopLeft);
	mViewport.TopLeftY = float(yTopLeft);
	mViewport.Width = float(nWidth);
	mViewport.Height = float(nHeight);
	mViewport.MinDepth = fMinZ;
	mViewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	mScissorRect.left = xLeft;
	mScissorRect.top = yTop;
	mScissorRect.right = xRight;
	mScissorRect.bottom = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->RSSetViewports(1, &mViewport);
	cmdList->RSSetScissorRects(1, &mScissorRect);
}

bool CCamera::IsInFrustum(const BoundingOrientedBox& boundingBox)
{
	return mFrustumWorld.Intersects(boundingBox);
}

bool CCamera::IsInFrustum(const BoundingBox& boundingBox)
{
	return mFrustumWorld.Intersects(boundingBox);
}

bool CCamera::IsInFrustum(const BoundingSphere& boundingSphere, int pass)
{
	if (pass == 0 || pass == 1)
	{
		return mFrustumWorld.Intersects(boundingSphere);
	}
	else if (pass == 2)
	{
		return mFrustumShadow.Intersects(boundingSphere);
	}
}

bool CCamera::IsInFrustum(std::shared_ptr<class CGameObject> obj)
{
	return mFrustumWorld.Intersects(obj->GetRootBoundingSphere());
}

const Vec3& CCamera::GetLocalPosition() 
{
	return GetTransform()->GetWorldPosition();
}
