#include "stdafx.h"
#include "Player.h"
#include"Shader.h"
#include"Light.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetLocalPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	if(camera)
		SetCameraOffset(cameraOffset);

	CGameObject::SetLocalPosition(x, y, z);
}

void CPlayer::SetLocalRotation(float x, float y, float z)
{
	pitch = x, yaw = y, roll = z;
}

void CPlayer::SetCameraOffset(const XMFLOAT3& xmf3CameraOffset)
{
	cameraOffset = xmf3CameraOffset;
	camera->SetLocalLookAt(Vector3::Add(position, cameraOffset), Vector3::Add(camera->GetLocalPosition(), look, 10.f), up);
	camera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, up, -fDistance);

		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity && Vector3::Length(velocity) < maxVelocity)
	{
		velocity = Vector3::Add(velocity, xmf3Shift);
		velocity = Vector3::ClampLength(velocity, 0.f, maxVelocity);
	}
	else
	{
		position = Vector3::Add(xmf3Shift, position);
		camera->Move(Vector3::ScalarProduct(xmf3Shift, 0.85f, false));
	}

}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);


}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	camera->Rotate(fPitch, fYaw, fRoll);
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

	//look = Vector3::Normalize(look);
	//right = Vector3::Normalize(Vector3::CrossProduct(up, look));
	//up = Vector3::Normalize(Vector3::CrossProduct(look, right));

}

void CPlayer::LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(position, xmf3LookAt, xmf3Up);
	right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{

	if (mainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * deltaTime);
		mainRotorFrame->transformMat = Matrix4x4::Multiply(xmmtxRotate, mainRotorFrame->transformMat);
	}
	if (tailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * deltaTime);
		tailRotorFrame->transformMat = Matrix4x4::Multiply(xmmtxRotate, tailRotorFrame->transformMat);
	}


	

	Move(velocity, false);


	/*if (isInvincible && invincibleTime < 1.f) {
		XMFLOAT3 dir = camera->GetRight();
		if (fmod(invincibleTime, 0.2f) > 0.1f) {
			camera->Move(Vector3::ScalarProduct(dir, 1.f - invincibleTime));
		}
		else {
			camera->Move(Vector3::ScalarProduct(dir, -(1.f - invincibleTime)));
		}
	}*/

	OnUpdateTransform();
	CGameObject::Update(deltaTime, pxmf4x4Parent);

	
	if (isInvincible)
	{
		if (invincibleTime < 2.0f) {
			invincibleTime += deltaTime;
		}
		else {
			invincibleTime = 0.f;
			isInvincible = false;
		}
	}

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(velocity, -1.0f));
	float fLength = Vector3::Length(velocity);
	float fDeceleration = friction * deltaTime;
	if (fDeceleration > fLength) fDeceleration = fLength;
	velocity = Vector3::Add(velocity, xmf3Deceleration, fDeceleration);

	camera->Update(this, position, deltaTime);
	camera->GenerateViewMatrix();
}

void CPlayer::OnUpdateTransform()
{
	transformMat._11 = right.x; transformMat._12 = right.y; transformMat._13 = right.z;
	transformMat._21 = up.x; transformMat._22 = up.y; transformMat._23 = up.z;
	transformMat._31 = look.x; transformMat._32 = look.y; transformMat._33 = look.z;
	transformMat._41 = position.x; transformMat._42 = position.y; transformMat._43 = position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	CGameObject::Render(cmdList, pCamera);
}

void CPlayer::InitPlayer()
{
	mainRotorFrame = FindFrame("rotor");
	tailRotorFrame = FindFrame("black_m_7");
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplanePlayer::CAirplanePlayer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	
}

CAirplanePlayer::~CAirplanePlayer()
{
	for (int i = 0; i < BULLETS; i++) 
		if (m_ppBullets[i]) delete m_ppBullets[i];
	;;
}

void CAirplanePlayer::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (isShieldOn) {
		if (shieldOnDelayTime < 3.f) {
			shieldOnDelayTime += deltaTime;
		}
		else {
			shieldOnDelayTime = 0.f;
			isShieldOn = false;
		}
	}

	CPlayer::Update(deltaTime);

	playerLight->SetDirection(look);
	playerLight->SetLocalPosition(position);

	if (isShieldOn && shieldMesh)
	{
		shieldMesh->oobs.Transform(shieldBS, XMLoadFloat4x4(&worldMat));
	}


	for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i]->active) m_ppBullets[i]->Update(deltaTime, pxmf4x4Parent);
	}
}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	float val1 = Vector3::DotProduct(right, velocity);
	float angle1 = val1 * -10.f;

	float val2 = Vector3::DotProduct(look, velocity);
	float angle2 = val2 * -5.f;

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(-XMConvertToRadians(angle2), 0.f, XMConvertToRadians(angle1));
	transformMat = Matrix4x4::Multiply(mtxRotate, transformMat);
}

void CAirplanePlayer::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	if ((fmod(invincibleTime, 0.2f) > 0.1f) || !isInvincible)
		CPlayer::Render(cmdList, pCamera);
	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->active) m_ppBullets[i]->Render(cmdList, pCamera);

	if (isShieldOn) {
		shieldShader->SetPipelineState(cmdList);

		CGameObject::Render(cmdList, &worldMat, shieldMaterial, shieldMesh);
	}

}

void CAirplanePlayer::ShieldOn()
{
	if (!isShieldOn && shieldOnDelayTime == 0.f) {
		isShieldOn = true;
	}
}

void CAirplanePlayer::SetMesh(std::shared_ptr<CMesh>& pMesh)
{
	mesh = pMesh; 
}

void CAirplanePlayer::FireBullet(std::shared_ptr<CGameObject>& pLockedObject)
{
/*
	if (pLockedObject) 
	{
		LookAt(pLockedObject->GetLocalPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		OnUpdateTransform();
	}
*/

	CBulletObject* pBulletObject = nullptr;
	for (int i = 0; i < BULLETS; i++)
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
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 2.0f, false));

		pBulletObject->transformMat = worldMat;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);

		if (pLockedObject)
		{
			pBulletObject->lockedObject = pLockedObject;
		}
	}
}

void CAirplanePlayer::SetShield(std::shared_ptr<CMesh>& pMesh, std::shared_ptr<CShader>& _shieldShader)
{
	if(pMesh) {
		shieldMesh = pMesh;
		shieldShader = _shieldShader;
		auto color = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
		shieldMaterial = std::make_shared<CMaterial>(color);
	}
}
