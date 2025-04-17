#pragma once
#include "Component.h"

struct CameraParams {
	Vec3 trackingPosition{};
    Vec2 framing{};
    float distance{};
    float pitch{};
    float yaw{};
};

struct CameraBlend {
	Vec2 framing;
	float pitch;
	float yaw;
	float distance;
};

class CThirdPersonCamera : public CComponent, public std::enable_shared_from_this<CThirdPersonCamera>
{
private:
	CameraParams mCameraParams;
	Vec2 mDeadZoneSize{};

	std::shared_ptr<class CCamera> mCamera;
	std::shared_ptr<class CGameObject> mTarget;
	std::shared_ptr<class CTerrain> mTerrain;

public:
	CThirdPersonCamera();
	CThirdPersonCamera(const CThirdPersonCamera& other) : CComponent(other) {}
	virtual ~CThirdPersonCamera();
	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CThirdPersonCamera>(*this); }

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetTarget(const std::shared_ptr<CGameObject>& target) { mTarget = target; }
	void SetTerrain(const std::shared_ptr<CTerrain>& terrain) { mTerrain = terrain; }
	CameraParams GetCameraParams() const { return mCameraParams; }

	void SetCameraParams(CameraParams& params);
	CameraParams GetCameraParams(Vec3 trackingPosition);

	CameraBlend ComputeBlendout();
	void SetParamsBlended(CameraParams camParams, CameraBlend blendout, float progress);

	void RaycastObjects();
};

