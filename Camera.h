#pragma once
#include"stdafx.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayer;

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

private:
	//월드 공간에서의 카메라 좌표계 속성
	XMFLOAT3 position{};
	XMFLOAT3 right{};
	XMFLOAT3 up{};
	XMFLOAT3 look{};

	//절두체 속성
	float nearZ{};
	float farZ{};
	float aspectRatio{};
	float fovAngle = 90.0f;
	float projectRectDist = 1.0f;


	BoundingFrustum frustumView = BoundingFrustum();
	BoundingFrustum frustumWorld = BoundingFrustum();
	XMFLOAT4X4 inverseViewMat = Matrix4x4::Identity();


public:
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	XMFLOAT4X4 viewMat = Matrix4x4::Identity();
	XMFLOAT4X4 perspectiveProjectMat = Matrix4x4::Identity();
	XMFLOAT4X4 viewPerspectiveProjectMat = Matrix4x4::Identity();

	XMFLOAT4X4 orthographicProjectMat = Matrix4x4::Identity();
	XMFLOAT4X4 viewOrthographicProjectMat = Matrix4x4::Identity();


public:
	void SetFOVAngle(float fFOVAngle);

	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle);
	void GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdList);

	void SetLookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);
	void SetLookAt(const XMFLOAT3& vPosition, const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);

	void Move(const XMFLOAT3& xmf3Shift);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(CPlayer* pPlayer, const XMFLOAT3& xmf3LookAt, float fTimeElapsed = 0.016f);

	bool IsInFrustum(const BoundingOrientedBox& xmBoundingBox);
	bool IsInFrustum(const BoundingBox& xmBoundingBox);

	XMFLOAT3 GetPosition() const { return position; } 
	XMFLOAT3 GetRight() const { return right; }
	XMFLOAT3 GetLook() const { return look; }
	XMFLOAT3 GetUp() const { return up; }

	float GetNear() const { return nearZ; }
	float GetFar() const { return farZ; }
	float GetAspect() const { return aspectRatio; }
	float GetFov() const { return fovAngle; }

	XMFLOAT4X4 GetViewMat() const { return viewMat; }
	XMFLOAT4X4 GetViewProjMat(); const
	XMFLOAT4X4 GetPerspectiveProjectMat() const { return perspectiveProjectMat; }
	XMFLOAT4X4 GetOrthographicProjectionMat() const { return orthographicProjectMat; }
};
