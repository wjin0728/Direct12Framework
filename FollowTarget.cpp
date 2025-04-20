#include "stdafx.h"
#include "FollowTarget.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"InputManager.h"
#include"Terrain.h"
#include "ServerManager.h"

CFollowTarget::CFollowTarget() : CMonoBehaviour("FollowTarget")
{
}

CFollowTarget::~CFollowTarget()
{
}

void CFollowTarget::Awake()
{
}

void CFollowTarget::Start()
{
}

void CFollowTarget::Update()
{
	auto targetTrans = mTarget->GetTransform();
	
	auto transform = GetTransform();
	uint8_t dir = 0;

	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		dir |= 0x08;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::S)) {
		dir |= 0x02;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::D)) {
		dir |= 0x01;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::A)) {
		dir |= 0x04;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::SHIFT)) {
		dir |= 0x20;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::CTRL)) {
		dir |= 0x10;
	}

	if (dir != 0) {
		INSTANCE(ServerManager).send_cs_move_packet(dir, transform->GetLocalLook());
	}

	// targetTrans->SetLocalPosition(targetTrans->GetLocalPosition() + acccel * DELTA_TIME);

	Vec2 mouseDelta = INPUT.GetMouseDelta();
	Vec3 targetPos = targetTrans->GetWorldPosition();

	if (mouseDelta != Vec2::Zero) {
		transform->RotateAround(targetPos, Vec3::Up, -mouseDelta.x * 0.01f);
		transform->RotateAround(targetPos, transform->GetWorldRight(), -mouseDelta.y * 0.01f);
		transform->LookAt(targetPos, Vec3::Up);
	}
	transform->GetWorldMat();
	Vec3 position = targetPos - transform->GetWorldLook() * 10.f;
	transform->SetLocalPosition(position);
}

void CFollowTarget::LateUpdate()
{
	auto targetTrans = mTarget->GetTransform();

	auto transform = GetTransform();

	//float terrainHeight = mTerrain->GetHeight(targetTrans->GetWorldPosition().x, targetTrans->GetWorldPosition().z);
}
