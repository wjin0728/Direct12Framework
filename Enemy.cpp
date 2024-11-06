#include"stdafx.h"
#include"Enemy.h"
#include"Player.h"

std::shared_ptr<CPlayer> CEnemy::targetPlayer;

CEnemy::CEnemy(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	
}

CEnemy::~CEnemy()
{
	for (int i = 0; i < ENEMY_BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CEnemy::SetLocalPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);

	CGameObject::SetLocalPosition(x, y, z);
}

void CEnemy::SetLocalRotation(float x, float y, float z)
{
	pitch = x, yaw = y, roll = z;
}

void CEnemy::LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(position, xmf3LookAt, xmf3Up);
	right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CEnemy::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity && (Vector3::Length(velocity) < maxVelocity))
	{
		velocity = Vector3::Add(velocity, xmf3Shift);
		velocity = Vector3::ClampLength(velocity, 0.f, maxVelocity);
	}
	else
	{
		position = Vector3::Add(xmf3Shift, position);
	}
}

void CEnemy::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CEnemy::Rotate(float fPitch, float fYaw, float fRoll)
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

void CEnemy::OnUpdateTransform()
{
	worldMat._11 = right.x; worldMat._12 = right.y; worldMat._13 = right.z;
	worldMat._21 = up.x; worldMat._22 = up.y; worldMat._23 = up.z;
	worldMat._31 = look.x; worldMat._32 = look.y; worldMat._33 = look.z;
	worldMat._41 = position.x; worldMat._42 = position.y; worldMat._43 = position.z;

	float val = Vector3::DotProduct(right, velocity);
	float angle = val * -15.f;

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(angle));
	worldMat = Matrix4x4::Multiply(mtxRotate, worldMat);
}

void CEnemy::Update(float deltaTime)
{
	if (active)
	{
		XMFLOAT3 dir = Vector3::Subtract(targetPlayer->position, this->position);
		float dist = Vector3::Length(dir);
		dir = Vector3::Normalize(dir);

		if (movingTime <= 0.01f) {
			std::random_device rd;
			std::default_random_engine dre(rd());
			std::uniform_real_distribution<float> randTime(0.f, 2.f);

			movingTime = randTime(dre);
			XMStoreFloat3(&movingDirection, RandomUnitVectorOnSphere());
		}
		else {
			XMFLOAT3 accel = Vector3::ScalarProduct(movingDirection, deltaTime * 4.f);
			Move(accel, true);
			movingTime -= deltaTime;
		}

		XMFLOAT3 lerpDir = Vector3::VectorLerp(dir, look, 0.8f);
		lerpDir = Vector3::Normalize(lerpDir);
		look = lerpDir;
		right = Vector3::CrossProduct({ 0.f,1.f,0.f }, look);
		up = Vector3::CrossProduct(look, right);

		if (targetPlayer && (dist > closedRange)) {
			XMFLOAT3 accel = Vector3::ScalarProduct(look, 0.3f);
			Move(accel, true);
		}

		Move(velocity, false);

		XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(velocity, -1.0f));
		float fLength = Vector3::Length(velocity);
		float fDeceleration = friction * deltaTime;
		if (fDeceleration > fLength) fDeceleration = fLength;
		velocity = Vector3::Add(velocity, xmf3Deceleration, fDeceleration);


		if (targetPlayer && (dist <= attackRange) && (afterFireTime >= bulletFireDelayTime)) {
			FireBullet();
			afterFireTime = 0.f;
		}
		else {
			afterFireTime += deltaTime;
		}
	}



	CExplosiveObject::Update(deltaTime);

	OnUpdateTransform();

	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		if (m_ppBullets[i]->active) m_ppBullets[i]->Update(deltaTime);
	}
}

void CEnemy::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	CExplosiveObject::Render(cmdList, pCamera);
	for (int i = 0; i < ENEMY_BULLETS; i++) if (m_ppBullets[i]->active) m_ppBullets[i]->Render(cmdList, pCamera);
}

void CEnemy::FireBullet()
{
	CBulletObject* pBulletObject = nullptr;
	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		if (!m_ppBullets[i]->active)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject)
	{
		XMFLOAT3 xmf3Position = GetLocalPosition();
		XMFLOAT3 xmf3Direction = GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->worldMat = worldMat;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);
	}
}

void CEnemy::SetTargetPlayer(std::shared_ptr<CPlayer>& player)
{
	targetPlayer = player;
}
