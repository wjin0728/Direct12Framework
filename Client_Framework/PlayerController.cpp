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
#include"ParticleManager.h"
#include "CutScene.h"
#include"RenderManager.h"
#include"UIRenderer.h"
#include"TargetMarker.h"
#include"ResourceManager.h"
#include"ParticleAttach.h"

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Awake()
{
	if (!rigidBody) rigidBody = GetOwner()->GetComponent<CRigidBody>();
	if (!mStateMachine) mStateMachine = owner->GetComponentFromHierarchy<CPlayerStateMachine>();

	auto targetUIObj = CGameObject::CreateUIObject("Sprite", "TargetMarker", { 0.f,0.f }, { 80.f,80.f });
	if (targetUIObj) {
		mTargetMarker = targetUIObj->AddComponent<CTargetMarker>();
		INSTANCE(CSceneManager).GetCurScene()->AddObject(targetUIObj);
	}
	mSkill = FIRE_EXPLOSION; // Default skill, can be changed later
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
	
	mTargetEnemy.reset();
	auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	auto& enemies = scene->GetObjectsWithType(OBJECT_TYPE::ENEMY);
	float minDistance = FLT_MAX;
	float sqMaxAttackRange = mMaxAttackRange * mMaxAttackRange;
	for (const auto& enemy : enemies) {
		BoundingSphere objBS = enemy->GetRootBoundingSphere();
		if (!camera->IsInFrustum(objBS, FORWARD)) continue;
		if (enemy->GetStateMachine()->mIsDead) continue;
		auto enemyTransform = enemy->GetTransform();
		Vec3 toEnemy = enemyTransform->GetWorldPosition() - GetTransform()->GetWorldPosition();
		toEnemy.y = 0.f;
		float distance = toEnemy.LengthSquared();
		if ((distance < sqMaxAttackRange) && (distance < minDistance)) {
			minDistance = distance;
			mTargetEnemy = enemy;
		}
	}
	mTargetMarker.lock()->SetTarget(mTargetEnemy.lock());
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

void CPlayerController::SetSkill(ITEM_TYPE skill)
{
	mSkill = skill;

}
void CPlayerController::SetState(PLAYER_STATE state)
{

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
			mStateMachine->ActivateShield(true);
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
			CastingSkill();
			return;
		}

		if (INPUT.IsKeyDown(KEY_TYPE::R)) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::ULTIMATE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ULTIMATE);
		}

		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		if(dir != 0) {
			INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
			mStateMachine->SetState((UINT8)PLAYER_STATE::RUN);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::RUN);
			moveKeyPressed = true;
			return;
		}
		break;
	case PLAYER_STATE::RUN:
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON)) {
			//INSTANCE(ServerManager).send_cs_mouse_ldown_packet(camForward);
			mStateMachine->SetState((UINT8)PLAYER_STATE::RUNATTACK);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::RUNATTACK);
			return;
		}

		if (INPUT.IsKeyDown(KEY_TYPE::SPACE)) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::JUMP);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::JUMP);
			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::F)) /*임시 아이템 생성*/ {
			//INSTANCE(ServerManager).send_cs_000_packet(0);
			mStateMachine->ActivateShield(true);
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
			CastingSkill();

			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::R)) {
			mStateMachine->SetState((UINT8)PLAYER_STATE::ULTIMATE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ULTIMATE);
		}

		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		if (dir == 0) {
			INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
			mStateMachine->SetState((UINT8)PLAYER_STATE::IDLE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::IDLE);
			return;
		}
		INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
		break;
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:

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

void CPlayerController::CastingSkill()
{
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
	{
		if (mTargetEnemy.lock()) {
			auto explosionPrefab = INSTANCE(CResourceManager).GetPrefab("Explosion");
			if (explosionPrefab) {
				auto explosionObj = CGameObject::Instantiate(explosionPrefab);

				auto camera = mCamera.lock()->GetTransform();
				Vec3 camForward = camera->GetWorldLook();
				Vec3 explosionPos = mTargetEnemy.lock()->GetRootBoundingSphere().Center;
				explosionPos -= camForward * 0.1f; 

				explosionObj->GetTransform()->SetLocalPosition(explosionPos);
				INSTANCE(CSceneManager).GetCurScene()->AddObject(explosionObj);
				explosionObj->Awake();
				explosionObj->Start();

				auto explosionParticle = explosionObj->GetComponent<CParticleAttach>();
				if (explosionParticle) {
					explosionParticle->Play();
				}
			}
			INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, mTargetEnemy.lock()->mID);
		}
		break;
	}
	case GRASS_VINE:
		if (mTargetEnemy.lock()) {
			INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, mTargetEnemy.lock()->mID);
		}
		break;
	}
}
