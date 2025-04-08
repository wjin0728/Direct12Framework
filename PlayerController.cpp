#include "stdafx.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "InputManager.h"
#include "RigidBody.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Terrain.h"
#include "ServerManager.h"

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Awake()
{

	rigidBody = GetOwner()->GetComponent<CRigidBody>();
}

void CPlayerController::Start()
{
}

void CPlayerController::Update()
{
	OnKeyEvents();
	OnMouseEvents();

}

void CPlayerController::LateUpdate()
{
}

void CPlayerController::OnKeyEvents()
{
	auto transform = GetTransform();
	Vec3 moveDir = Vec3::Zero;
	uint8_t dir = 0;

	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		moveDir += transform->GetLocalLook();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x08;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::A)) {
		moveDir -= transform->GetLocalRight();
		moveDir.y = 0.f;
		moveDir.Normalize();

		dir |= 0x04;
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
		acccel = moveDir.GetNormalized() * 20.f;
		isDecelerate = false;
	}

	rigidBody->SetAcceleration(acccel);
	rigidBody->SetUseFriction(isDecelerate);

}

void CPlayerController::OnMouseEvents()
{
	Vec2 mouseDelta = INPUT.GetMouseDelta();
	if (mouseDelta != Vec2::Zero) {
		GetTransform()->RotateY(mouseDelta.x * DELTA_TIME * 30);
	}

}
