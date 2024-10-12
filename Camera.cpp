#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "Player.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"UploadBuffer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

void CCamera::GenerateViewMatrix()
{
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(Vector3::CrossProduct(up, look));
	up = Vector3::Normalize(Vector3::CrossProduct(look, right));

	viewMat._11 = right.x; viewMat._12 = up.x; viewMat._13 = look.x;
	viewMat._21 = right.y; viewMat._22 = up.y; viewMat._23 = look.y;
	viewMat._31 = right.z; viewMat._32 = up.z; viewMat._33 = look.z;
	viewMat._41 = -Vector3::DotProduct(position, right);
	viewMat._42 = -Vector3::DotProduct(position, up);
	viewMat._43 = -Vector3::DotProduct(position, look);

	viewPerspectiveProjectMat = Matrix4x4::Multiply(viewMat, perspectiveProjectMat);
	viewOrthographicProjectMat = Matrix4x4::Multiply(viewMat, orthographicProjectMat);

	inverseViewMat._11 = right.x; inverseViewMat._12 = right.y; inverseViewMat._13 = right.z;
	inverseViewMat._21 = up.x; inverseViewMat._22 = up.y; inverseViewMat._23 = up.z;
	inverseViewMat._31 = look.x; inverseViewMat._32 = look.y; inverseViewMat._33 = look.z;
	inverseViewMat._41 = position.x; inverseViewMat._42 = position.y; inverseViewMat._43 = position.z;

	frustumView.Transform(frustumWorld, XMLoadFloat4x4(&inverseViewMat));
}

void CCamera::SetLookAt(const XMFLOAT3& xmf3Position, const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	position = xmf3Position;
	viewMat = Matrix4x4::LookAtLH(position, xmf3LookAt, xmf3Up);
	right = Vector3::Normalize(XMFLOAT3(viewMat._11, viewMat._21, viewMat._31));
	up = Vector3::Normalize(XMFLOAT3(viewMat._12, viewMat._22, viewMat._32));
	look = Vector3::Normalize(XMFLOAT3(viewMat._13, viewMat._23, viewMat._33));
}

void CCamera::SetLookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(position, xmf3LookAt, xmf3Up);
	right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetFOVAngle(float fFOVAngle)
{
	fovAngle = fFOVAngle;
	projectRectDist = float(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{
	float fAspectRatio = (float(viewport.Width) / float(viewport.Height));
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&perspectiveProjectMat, xmmtxProjection);

	nearZ = fNearPlaneDistance;
	farZ = fFarPlaneDistance;
	fovAngle = fFOVAngle;

	BoundingFrustum::CreateFromMatrix(frustumView, xmmtxProjection);
}

void CCamera::GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(fWidth, hHeight, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&orthographicProjectMat, xmmtxProjection);
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	viewport.TopLeftX = float(xTopLeft);
	viewport.TopLeftY = float(yTopLeft);
	viewport.Width = float(nWidth);
	viewport.Height = float(nHeight);
	viewport.MinDepth = fMinZ;
	viewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	scissorRect.left = xLeft;
	scissorRect.top = yTop;
	scissorRect.right = xRight;
	scissorRect.bottom = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissorRect);
}

bool CCamera::IsInFrustum(const BoundingOrientedBox& xmBoundingBox)
{
	return(frustumWorld.Intersects(xmBoundingBox));
}

bool CCamera::IsInFrustum(const BoundingBox& xmBoundingBox)
{
	return(frustumWorld.Intersects(xmBoundingBox));
}

XMFLOAT4X4 CCamera::GetViewProjMat()
{
	GenerateViewMatrix();
	return Matrix4x4::Multiply(viewMat, perspectiveProjectMat);
}

void CCamera::Move(const XMFLOAT3& xmf3Shift)
{
	position = Vector3::Add(position, xmf3Shift);
}

void CCamera::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(fPitch));
		look = Vector3::TransformNormal(look, mtxRotate);
		up = Vector3::TransformNormal(up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(fYaw));
		look = Vector3::TransformNormal(look, mtxRotate);
		right = Vector3::TransformNormal(right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(fRoll));
		up = Vector3::TransformNormal(up, mtxRotate);
		right = Vector3::TransformNormal(right, mtxRotate);
	}
}

void CCamera::Update(CPlayer* pPlayer, const XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::Identity();
	mtxRotate._11 = pPlayer->right.x; mtxRotate._21 = pPlayer->up.x; mtxRotate._31 = pPlayer->look.x;
	mtxRotate._12 = pPlayer->right.y; mtxRotate._22 = pPlayer->up.y; mtxRotate._32 = pPlayer->look.y;
	mtxRotate._13 = pPlayer->right.z; mtxRotate._23 = pPlayer->up.z; mtxRotate._33 = pPlayer->look.z;

	XMFLOAT3 xmf3Offset = Vector3::TransformCoord(pPlayer->cameraOffset, mtxRotate);
	XMFLOAT3 xmf3Position = Vector3::Add(pPlayer->position, xmf3Offset);
	XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, position);
	float fLength = Vector3::Length(xmf3Direction);
	xmf3Direction = Vector3::Normalize(xmf3Direction);

	float fTimeLagScale = fTimeElapsed * 10.f;
	float fDistance = fLength * fTimeLagScale;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0)
	{
		position = Vector3::Add(position, xmf3Direction, fDistance);
		SetLookAt(Vector3::Add(position, pPlayer->look, 10.f), pPlayer->up);
	}
}
