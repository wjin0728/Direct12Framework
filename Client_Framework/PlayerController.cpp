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
#include "AnimationEnums.h"
#include "ObjectState.h"

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Awake()
{
	rigidBody = GetOwner()->GetComponent<CRigidBody>();
	mStateMachine = owner->GetComponentFromHierarchy<CPlayerStateMachine>();
}

void CPlayerController::Start()
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();
	SetClass(mStateMachine->GetClass());
}

void CPlayerController::Update()
{
	if (INPUT.IsKeyDown(KEY_TYPE::F1)) {
		if (!mFreeLook) {
			if (moveKeyPressed == true) {
				moveKeyPressed = false;
				auto camera = mCamera.lock()->GetTransform();
				Vec3 camForward = camera->GetWorldLook();
				INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
				mStateMachine->SetState((UINT8)PLAYER_STATE::IDLE);
				INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			}
		}
		mFreeLook = !mFreeLook;
	}

	if (mFreeLook) {
		// Handle free look camera logic here if needed
		return;
	}
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
}

void CPlayerController::SetState(PLAYER_STATE state)
{

}

UINT8 CPlayerController::GetAnimationIndexFromState(PLAYER_STATE state)
{
	switch (mClass)
	{
	case PLAYER_CLASS::ARCHER:
		return (UINT8)CAnimationController::ARCHER_MAP.at(state);
	case PLAYER_CLASS::FIGHTER:
		return (UINT8)CAnimationController::FIGHTER_MAP.at(state);
	case PLAYER_CLASS::MAGE:
		return (UINT8)CAnimationController::MAGE_MAP.at(state);
	default:
		return 0;
	}
}

void CPlayerController::OnKeyEvents()
{
	auto transform = GetTransform();
	auto camera = mCamera.lock()->GetTransform();
	uint8_t dir = 0;

	Vec3 camForward = camera->GetWorldLook();
	Vec3 camRight = camera->GetWorldRight();
	camForward.y = 0.f;
	camRight.y = 0.f;
	camForward.Normalize();
	camRight.Normalize();

	PLAYER_STATE currentState = (PLAYER_STATE)mStateMachine->GetState();

	switch (currentState)
	{
	case PLAYER_STATE::IDLE:
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON)) {
			INSTANCE(ServerManager).send_cs_mouse_ldown_packet(camForward);
			mStateMachine->SetState((UINT8)PLAYER_STATE::ATTACK);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ATTACK);
			return;
		}
	case PLAYER_STATE::RUN:
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON)) {
			INSTANCE(ServerManager).send_cs_mouse_ldown_packet(camForward);
			mStateMachine->SetState((UINT8)PLAYER_STATE::ATTACK);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ATTACK);
			return;
		}

		if (INPUT.IsKeyDown(KEY_TYPE::SPACE)) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::JUMP);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::JUMP);
			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::F)) /*임시 아이템 생성*/ {
			INSTANCE(ServerManager).send_cs_000_packet(0);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::M)) /*임시 적 생성*/ {
			INSTANCE(ServerManager).send_cs_000_packet(1);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::ONE)) /*임시 씬 전환*/ {
			INSTANCE(ServerManager).send_cs_000_packet(2);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::TWO)) /*임시 씬 전환*/ {
			INSTANCE(ServerManager).send_cs_000_packet(3);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::THREE)) /*임시 씬 전환*/ {
			INSTANCE(ServerManager).send_cs_000_packet(4);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::E)) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::SKILL);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::SKILL);
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
				//INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, Ÿ��id);
				break;
			}
			return;
		}
		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		if (dir == 0) {
			if (moveKeyPressed == true) {
				moveKeyPressed = false;
				INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
				mStateMachine->SetState((UINT8)PLAYER_STATE::IDLE);
				INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			}
			return;
		}
		else if (moveKeyPressed == false) {
			moveKeyPressed = true;
		}
		INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
		if (mStateMachine->GetState() == (UINT8)PLAYER_STATE::IDLE) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::RUN);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::RUN);
		}
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::MOVE_ATTACK:
		break;
	case PLAYER_STATE::GETHIT:
		break;
	case PLAYER_STATE::DEATH:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::SKILL:
		break;
	case PLAYER_STATE::end:
		break;
	default:
		break;
	}

	
}