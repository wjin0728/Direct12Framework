#pragma once
#include"stdafx.h"
#include"GameObject.h"

class CEnemy : public CExplosiveObject
{
private:
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 cameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float friction = 3.0f;
	float maxVelocity = 2.f;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	float bulletRange = 700.0f;
	float bulletFireDelayTime = 3.f;
	float afterFireTime = 0.f;



	float closedRange = 150.f;
	float attackRange = 180.f;
	static std::shared_ptr<CPlayer> targetPlayer;

	CGameObject* mainRotorFrame;
	CGameObject* tailRotorFrame;

public:
	CBulletObject* m_ppBullets[ENEMY_BULLETS];

	CEnemy(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	virtual ~CEnemy();

	void SetFirstFireTime(float afterFireTime_) { afterFireTime = afterFireTime_; }

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);

	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	virtual void OnUpdateTransform();
	virtual void Update(float deltaTime);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);

	void FireBullet();

	static void SetTargetPlayer(std::shared_ptr<CPlayer>& player);
};
