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
#include"Animation.h"
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
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();
}

void CPlayerController::Update()
{
	OnKeyEvents();
	// auto transform = GetTransform();
	// float terrainHeight = mTerrain.lock()->GetHeight(transform->GetWorldPosition().x, transform->GetWorldPosition().z);

	// Vec3 pos = transform->GetWorldPosition();
	// pos.y = terrainHeight;
	// transform->SetLocalPosition(pos);
}

void CPlayerController::LateUpdate()
{
}

void CPlayerController::SetChildAnimationController()
{
	mAnimationController = owner->GetComponentFromHierarchy<CAnimationController>();
}

void CPlayerController::SetState(PLAYER_STATE state)
{

}

void CPlayerController::OnKeyEvents()
{
	auto transform = GetTransform();
	auto camera = mCamera.lock()->GetTransform();
	Vec3 moveDir = Vec3::Zero;
	uint8_t dir = 0;

	Vec3 camForward = camera->GetWorldLook();
	Vec3 camRight = camera->GetWorldRight();
	camForward.y = 0.f;
	camRight.y = 0.f;
	camForward.Normalize();
	camRight.Normalize();

	if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
	if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
	if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
	if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;
	if (INPUT.IsKeyPress(KEY_TYPE::CTRL)) dir |= 0x10;

	if (INPUT.IsKeyDown(KEY_TYPE::SPACE)) {
		INSTANCE(ServerManager).send_cs_000_packet();
	}
	if (INPUT.IsKeyDown(KEY_TYPE::E)) {
		switch (mSkill)
		{
		case FIRE_ENCHANT:
		case WATER_HEAL:
		case WATER_SHIELD:
		case GRASS_WEAKEN:
			INSTANCE(ServerManager).send_cS_skill_nontarget_packet(mSkill);
			break;
		case FIRE_EXPLOSION:
		case GRASS_VINE:
			//INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, Å¸°Ùid);
			break;
		}
	}

	if (dir == 0) return;
	INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
}