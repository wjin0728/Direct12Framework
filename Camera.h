#pragma once
#include"stdafx.h"
#include"Component.h"

class CCamera : public CComponent
{
public:
	CCamera();
	virtual ~CCamera();

private:
	
	float mNearZ{};
	float mFarZ{};
	float mAspectRatio{};
	float mFovAngle = 90.0f;
	float mProjectRectDist = 1.0f;
		  
	BoundingFrustum mFrustumView = BoundingFrustum();
	BoundingFrustum mFrustumWorld = BoundingFrustum();
	Matrix mInverseViewMat = Matrix::Identity;


public:
	D3D12_VIEWPORT mViewport{};
	D3D12_RECT mScissorRect{};

	Matrix mViewMat = Matrix::Identity;
	Matrix mPerspectiveProjectMat = Matrix::Identity;
	Matrix mViewPerspectiveProjectMat = Matrix::Identity;

	Matrix mOrthographicProjectMat = Matrix::Identity;
	Matrix mViewOrthographicProjectMat = Matrix::Identity;

public:
	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

public:
	void SetFOVAngle(float fovAngle);

	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float nearPlane, float farPlane, float fovAngle);
	void GenerateOrthographicProjectionMatrix(float nearPlane, float farPlane, float width, float height);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdList);

	bool IsInFrustum(const BoundingOrientedBox& boundingBox);
	bool IsInFrustum(const BoundingBox& boundingBox);

	float GetNear() const { return mNearZ; }
	float GetFar() const { return mFarZ; }
	float GetAspect() const { return mAspectRatio; }
	float GetFov() const { return mFovAngle; }

	Matrix GetViewMat() const { return mViewMat; }
	Matrix GetViewProjMat() const { return mViewPerspectiveProjectMat; }
	Matrix GetPerspectiveProjectMat() const { return mPerspectiveProjectMat; }
	Matrix GetOrthographicProjectionMat() const { return mOrthographicProjectMat; }
};
