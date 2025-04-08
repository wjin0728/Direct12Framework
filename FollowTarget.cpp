#include "stdafx.h"
#include "FollowTarget.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "InputManager.h"
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
	Vec3 moveDir = Vec3::Zero;
	uint8_t dir = 0;

	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		moveDir += transform->GetLocalLook();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x08;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::S)) {
		moveDir -= transform->GetLocalLook();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x02;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::D)) {
		moveDir += transform->GetLocalRight();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x01;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::A)) {
		moveDir -= transform->GetLocalRight();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x04;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::SHIFT)) {
		moveDir += Vec3::Up;

		dir |= 0x20;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::CTRL)) {
		moveDir -= Vec3::Up;

		dir |= 0x10;
	}

	if (dir != 0) {
		INSTANCE(ServerManager).send_cs_move_packet(dir);
	}

	Vec3 acccel = Vec3::Zero;
	bool isDecelerate = true;

	if (moveDir != Vec3::Zero) {
		acccel = moveDir.GetNormalized() * 10.f;
		isDecelerate = false;
	}

	targetTrans->SetLocalPosition(targetTrans->GetLocalPosition() + acccel * DELTA_TIME);


	Vec2 mouseDelta = INPUT.GetMouseDelta();
	if (mouseDelta != Vec2::Zero) {
		GetTransform()->RotateAround(targetTrans->GetWorldPosition(), Vec3::Up, mouseDelta.x * 0.01f);
		GetTransform()->RotateAround(targetTrans->GetWorldPosition(), transform->GetWorldRight(), mouseDelta.y * 0.01f);
		GetTransform()->LookAt(targetTrans->GetWorldPosition(), Vec3::Up);
		GetTransform()->SetLocalUp(Vec3::Up);
	}
}

void CFollowTarget::LateUpdate()
{
	auto targetTrans = mTarget->GetTransform();

	auto transform = GetTransform();

	Vec3 position = targetTrans->GetWorldPosition() - transform->GetLocalLook() * 10.f;
	transform->SetLocalPosition(position);
}
