#include "stdafx.h"
#include "PlayerController.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"InputManager.h"
#include"RigidBody.h"

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Awake()
{
	auto mainRotorObj = GetOwner()->FindChildByName(L"MainRotor");

	if (mainRotorObj) {
		mainRotor = mainRotorObj->GetTransform();
	}

	auto subRotorObj = GetOwner()->FindChildByName(L"TailRotor");

	if (subRotorObj) {
		subRotor = subRotorObj->GetTransform();
	}

	rigidBody = std::static_pointer_cast<CRigidBody>(GetOwner()->GetComponent(COMPONENT_TYPE::RIGID_BODY));
}

void CPlayerController::Start()
{
}

void CPlayerController::Update()
{
	bool isDecelerate = false;

	Vec3 acccel = GetTransform()->GetLocalLook() * 20.f;

	rigidBody->SetAcceleration(acccel);
	rigidBody->SetUseFriction(isDecelerate);

	float power = DELTA_TIME * 3000;

	if (mainRotor) {
		mainRotor->Rotate({ 0.f, power , 0.f });
	}
	if (subRotor) {
		subRotor->Rotate({ power, 0.f , 0.f });
	}
}

void CPlayerController::LateUpdate()
{
}

void CPlayerController::OnKeyEvents()
{
	auto transform = GetTransform();
	Vec3 moveDir = Vec3::Zero;

	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		moveDir += transform->GetLocalLook();
	}
	if (INPUT.IsKeyPress(KEY_TYPE::S)) {
		moveDir -= transform->GetLocalLook();
	}
	if (INPUT.IsKeyPress(KEY_TYPE::D)) {
		moveDir += transform->GetLocalRight();
	}
	if (INPUT.IsKeyPress(KEY_TYPE::A)) {
		moveDir -= transform->GetLocalRight();
	}
	if (INPUT.IsKeyPress(KEY_TYPE::SHIFT)) {
		moveDir += transform->GetLocalUp();
	}
	if (INPUT.IsKeyPress(KEY_TYPE::CTRL)) {
		moveDir -= transform->GetLocalUp();
	}

	Vec3 acccel = Vec3::Zero;
	bool isDecelerate = true;

	if (moveDir != Vec3::Zero) {
		acccel = moveDir.GetNormalized() * 20.f;
		isDecelerate = false;
	}

	rigidBody->SetAcceleration(acccel);
	rigidBody->SetUseFriction(isDecelerate);

	/*Vec3 velocity = rigidBody->GetVelocity();

	float val1 = GetTransform()->GetLocalRight().Dot(velocity);
	float angle1 = val1 * -10.f;

	float val2 = GetTransform()->GetLocalLook().Dot(velocity);
	float angle2 = val2 * -5.f;

	Vec3 rotation 
	GetTransform()->SetLocalRotation(angle2, 0.f, angle1);*/
}

void CPlayerController::OnMouseEvents()
{
	/*Vec2 mouseDelta = INPUT.GetMouseDelta();
	if (mouseDelta != Vec2::Zero) {
		GetTransform()->Rotate({ 0.f , mouseDelta.x * DELTA_TIME * 60, 0.f });
	}*/

}
