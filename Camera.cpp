#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include"GameObject.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"UploadBuffer.h"
#include"Transform.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera() : CComponent(COMPONENT_TYPE::CAMERA)
{
}

CCamera::~CCamera()
{
}

void CCamera::GenerateViewMatrix()
{
	auto transform = owner.lock()->GetTransform(); 
	Vec3 position = transform->mPosition;
	Vec3 right = transform->mRight;
	Vec3 up = transform->mUp;
	Vec3 look = transform->mLook;

	mViewMat._11 = right.x; mViewMat._12 = up.x; mViewMat._13 = look.x;
	mViewMat._21 = right.y; mViewMat._22 = up.y; mViewMat._23 = look.y;
	mViewMat._31 = right.z; mViewMat._32 = up.z; mViewMat._33 = look.z;
	mViewMat._41 = -(position.Dot(right));
	mViewMat._42 = -(position.Dot(up));
	mViewMat._43 = -(position.Dot(look));

	mViewPerspectiveProjectMat = mViewMat * mPerspectiveProjectMat;

	mInverseViewMat._11 = right.x; mInverseViewMat._12 = right.y; mInverseViewMat._13 = right.z;
	mInverseViewMat._21 = up.x; mInverseViewMat._22 = up.y; mInverseViewMat._23 = up.z;
	mInverseViewMat._31 = look.x; mInverseViewMat._32 = look.y; mInverseViewMat._33 = look.z;
	mInverseViewMat._41 = position.x; mInverseViewMat._42 = position.y; mInverseViewMat._43 = position.z;

	mFrustumWorld.Transform(mFrustumWorld, mInverseViewMat);
}

void CCamera::Awake()
{

}

void CCamera::Start()
{
}

void CCamera::Update()
{
}

void CCamera::LateUpdate()
{
	auto transform = owner.lock()->GetTransform();

	if (transform->isMoved) {
		GenerateViewMatrix();
	}
}

void CCamera::FixedUpdate()
{
}

void CCamera::SetFOVAngle(float fovAngle)
{
	mFovAngle = fovAngle;
	mProjectRectDist = float(1.0f / tan(XMConvertToRadians(mFovAngle * 0.5f)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float nearPlane, float farPlane, float fovAngle)
{
	mAspectRatio = (float(mViewport.Width) / float(mViewport.Height));
	mPerspectiveProjectMat = Matrix::CreatePerspectiveFieldOfView(fovAngle, mAspectRatio, nearPlane, farPlane);

	mNearZ = nearPlane;
	mFarZ = farPlane;
	mFovAngle = fovAngle;

	BoundingFrustum::CreateFromMatrix(mFrustumView, mPerspectiveProjectMat);
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
