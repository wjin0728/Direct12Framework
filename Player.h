#pragma once

#include "GameObject.h"
#include "Camera.h"

class CSpotLight;

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 cameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float friction = 8.0f;
	float maxVelocity = 3.f;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	float invincibleTime = 0.f;
	bool isInvincible = false;

	std::shared_ptr<CCamera> camera;

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);

	void Move(DWORD dwDirection, float fDistance);
	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void SetCameraOffset(const XMFLOAT3& xmf3CameraOffset);

	virtual void OnUpdateTransform();
	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);

	void BeInvincible() { if (!isInvincible) isInvincible = true; }

	void SetCamera(std::shared_ptr<CCamera>& _camera) { camera = _camera; }
	std::shared_ptr<CCamera>& GetCamera() { return camera; }

	void InitPlayer();

	CGameObject* mainRotorFrame;
	CGameObject* tailRotorFrame;
};

#define BULLETS 50

class CShader;

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	virtual ~CAirplanePlayer();

	float maxSpeed = 200.f;

	float bulletRange = 1500.0f;
	CBulletObject* m_ppBullets[BULLETS];

	void SetMesh(std::shared_ptr<CMesh>& pMesh);
	void SetShield(std::shared_ptr<CMesh>& pMesh, std::shared_ptr<CShader>& _shieldShader);
	void SetPlayerLight(std::shared_ptr<CSpotLight>& light) { playerLight = light; }
	void FireBullet(std::shared_ptr<CGameObject>& pLockedObject);

	virtual void OnUpdateTransform();
	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);

	bool IsShieldOn() { return isShieldOn; }
	void ShieldOn();

	BoundingSphere shieldBS = BoundingSphere();

private:
	std::shared_ptr<CMesh> shieldMesh = nullptr;
	std::shared_ptr<CMaterial> shieldMaterial;
	std::shared_ptr<CShader> shieldShader;
	std::shared_ptr<CSpotLight> playerLight;

	float shieldOnDelayTime = 0.f;
	bool isShieldOn = false;

};

