#pragma once
#include"stdafx.h"

class ICameraState
{
protected:
	std::weak_ptr<class CCamera> mCamera{};


public:
	ICameraState(std::shared_ptr<class CCamera> camera) : mCamera(camera) {}
	virtual ~ICameraState() = default;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update() = 0;
	virtual void HandleInput() = 0;
	virtual bool IsActive() const = 0;
	virtual bool IsTransitioning() const = 0;
};

class CFreeCameraState : public ICameraState
{
	public:
	CFreeCameraState(std::shared_ptr<class CCamera> camera) : ICameraState(camera) {}
	void Enter() override;
	void Exit() override;
	void Update() override;
	void HandleInput() override;
	bool IsActive() const override { return true; }
	bool IsTransitioning() const override { return false; }
};

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

class CTargetCameraState : public ICameraState
{
private:
	CameraParams mCameraParams;
	Vec2 mDeadZoneSize{};
	Vec3 mOriginalPosition{};
	bool mIsHit{ false };
	bool mCanRotate{ true };
	std::shared_ptr<class CGameObject> mTarget{};

public:
	CTargetCameraState(std::shared_ptr<class CCamera> camera) : ICameraState(camera) {}
	void Enter() override;
	void Exit() override;
	void Update() override;
	void HandleInput() override;
	bool IsActive() const override { return true; }
	bool IsTransitioning() const override { return false; }
};

class CCutsceneCameraState : public ICameraState
{
private:
	std::shared_ptr<class CGameObject> mTarget{};

public:
	CCutsceneCameraState(std::shared_ptr<class CCamera> camera, std::shared_ptr<class CCutscene> cutscene) 
		: ICameraState(camera) {}
	void Enter() override;
	void Exit() override;
	void Update() override;
	void HandleInput() override;
	bool IsActive() const override { return true; }
	bool IsTransitioning() const override { return false; }
};
