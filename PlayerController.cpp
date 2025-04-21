#include "stdafx.h"
#include "PlayerController.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"InputManager.h"
#include"RigidBody.h"
#include"SceneManager.h"
#include"Scene.h"
#include"Terrain.h"
#include"Camera.h"

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Awake()
{
	rigidBody = GetOwner()->GetComponent<CRigidBody>();
}

void CPlayerController::Start()
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();
}

void CPlayerController::Update()
{
	OnKeyEvents();
	auto transform = GetTransform();
	float terrainHeight = mTerrain.lock()->GetHeight(transform->GetWorldPosition().x, transform->GetWorldPosition().z);

	Vec3 pos = transform->GetWorldPosition();
	pos.y = terrainHeight;
	transform->SetLocalPosition(pos);
}

void CPlayerController::LateUpdate()
{
}

void CPlayerController::OnKeyEvents()
{
	auto transform = GetTransform();
	auto camera = mCamera.lock()->GetTransform();
	Vec3 moveDir = Vec3::Zero;

	Vec3 camForward = camera->GetWorldLook();
	Vec3 camRight = camera->GetWorldRight();
	camForward.y = 0.f;
	camRight.y = 0.f;
	camForward.Normalize();
	camRight.Normalize();

	if (INPUT.IsKeyPress(KEY_TYPE::W)) {
		moveDir += camForward;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::S)) {
		moveDir -= camForward;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::D)) {
		moveDir += camRight;
	}
	if (INPUT.IsKeyPress(KEY_TYPE::A)) {
		moveDir -= camRight;
	}

	Vec3 accel = Vec3::Zero;
	bool isDecelerate = true;

	if (moveDir != Vec3::Zero) {
		moveDir.Normalize();

		Vec3 currentVelocity = rigidBody->GetVelocity();
		float currentSpeed = currentVelocity.Length();
		if (currentVelocity != Vec3::Zero) {
			Vec3 currentDir = currentVelocity.GetNormalized();
			float dot = currentDir.Dot(moveDir);

			if (dot < 0.0f) {
				accel = -currentDir * 20.f; 
			}
			else {
				rigidBody->SetVelocity(currentSpeed * dot * moveDir);
				accel = moveDir * 10.f;
				isDecelerate = false;
			}
		}
		else {
			accel = moveDir * 10.f; // 정지 상태에서 가속
			isDecelerate = false;
		}
	}

	rigidBody->SetAcceleration(accel);
	rigidBody->SetUseFriction(isDecelerate);

	float rotationSpeed = 10.f;

	if (moveDir.LengthSquared() > 0.001f)
	{
		Quaternion targetRot = Quaternion::LookRotation(moveDir);
		Quaternion rotation = Quaternion::Slerp(transform->GetLocalRotation(), targetRot, rotationSpeed * DELTA_TIME);
		transform->SetLocalRotation(rotation);
	}
}