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
	mSkill = item_end; // Default skill, can be changed later
}

void CPlayerController::Start()
{
	if (!mStateMachine) mStateMachine = owner->GetComponentFromHierarchy<CPlayerStateMachine>();

	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();

	if(mStateMachine) SetClass(mStateMachine->GetClass());

	if (auto controller = owner->GetComponentFromHierarchy<CAnimationController>()) {
		auto func = [](float time) {
			INSTANCE(ServerManager).send_cs_attack_packet();
			std::cout << "Do attack packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Attack", "Attack", func);
		controller->AddAnimationEvent("RunAttack", "Attack", func);

		auto func2 = [this](float time) {
			CastingSkill();
			std::cout << "Do skill packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Skill", "Skill", func2);

		auto func3 = [this](float time) {
			if (!mTargetEnemy.lock()) return;
			INSTANCE(ServerManager).send_cs_ultimate_skill_packet();
			std::cout << "Do ultimate skill packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Ultimate", "Ultimate", func3);
	}
	mUltimateSkillCooldownTime = 5.f;
	mUltimateSkillCooldown = 0.f;
}

void CPlayerController::Update()
{
	
	switch (mControllMode)
	{
	case CPlayerController::ControllMode::None:
		break;
	case CPlayerController::ControllMode::FreeLook:
		break;
	case CPlayerController::ControllMode::LockOn:
	{
		LockOnTarget();
		InteractWithItem();
		OnKeyEvents();

		PLAYER_STATE currentState = (PLAYER_STATE)mStateMachine->GetState();
		if (currentState != PLAYER_STATE::ULTIMATE) {
			mUltimateSkillCooldown += DELTA_TIME;
			if (mUltimateSkillCooldown > mUltimateSkillCooldownTime) {
				mUltimateSkillCooldown = mUltimateSkillCooldownTime;
			}
		}
		owner->TriggerEvent("OnUltimateSkillCooldown", { 1 - (mUltimateSkillCooldown / mUltimateSkillCooldownTime) });
	}
		break;
	case CPlayerController::ControllMode::ClassSelection:
		break;
	default:
		break;
	}
	

	if (INPUT.IsKeyDown(KEY_TYPE::F1)) {
		if (mControllMode == ControllMode::LockOn)
			ChangeControllMode(ControllMode::FreeLook);
	}
	else if (INPUT.IsKeyDown(KEY_TYPE::F2)) {
		if(mControllMode == ControllMode::FreeLook)
			ChangeControllMode(ControllMode::LockOn);
	}


}

void CPlayerController::LockOnTarget()
{
	mTargetEnemy.reset();
	auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	auto& enemies = scene->GetObjectsWithType(OBJECT_TYPE::ENEMY);
	float minDistance = FLT_MAX;
	float sqMaxAttackRange = mMaxAttackRange * mMaxAttackRange;
	for (const auto& enemy : enemies) {
		BoundingSphere objBS = enemy->GetRootBoundingSphere();
		if (!camera->IsInFrustum(objBS, FORWARD)) continue;
		auto enemyState = enemy->GetStateMachine();
		if (enemyState->mIsDead || !enemyState->mIsSpawningFinished) continue;
		auto enemyTransform = enemy->GetTransform();
		Vec3 toEnemy = enemyTransform->GetWorldPosition() - GetTransform()->GetWorldPosition();
		toEnemy.y = 0.f;
		float distance = toEnemy.LengthSquared();
		if ((distance < sqMaxAttackRange) && (distance < minDistance)) {
			minDistance = distance;
			mTargetEnemy = enemy;
		}
	}
}

void CPlayerController::InteractWithItem()
{
	auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	auto& items = scene->GetObjectsWithType(OBJECT_TYPE::ITEM);
	float maxInteractRange = 1.f; // Maximum interaction range
	float sqMaxInteractRange = maxInteractRange * maxInteractRange;
	float minDistance = FLT_MAX;

	if (mTargetItem.expired()) {
		owner->TriggerEvent("OnItemTargeted", { false, Vec2(0.f, 0.f) });
	}
	auto prevTargetItem = mTargetItem.lock();
	mTargetItem.reset();
	for (const auto& item : items) {
		BoundingSphere objBS = item->GetRootBoundingSphere();
		if (!camera->IsInFrustum(objBS, FORWARD)) continue;
		auto itemTransform = item->GetTransform();
		Vec3 toItem = itemTransform->GetWorldPosition() - GetTransform()->GetWorldPosition();
		toItem.y = 0.f;
		float distance = toItem.LengthSquared();
		if (distance < minDistance && distance < sqMaxInteractRange) {
			minDistance = distance;
			mTargetItem = item;
		}
	}

	if(auto targetItem = mTargetItem.lock()) {
		Vec3 itemPos = targetItem->GetTransform()->GetWorldPosition();
		Vec3 playerPos = GetTransform()->GetWorldPosition();
		playerPos.y += 0.7f; 
		Vec2 itemPosCS = camera->TransformToNDC(itemPos);
		Vec2 playerPosCS = camera->TransformToNDC(playerPos);

		float offset = 0.2f;

		if (itemPosCS.x < playerPosCS.x) {
			playerPosCS.x -= offset; 
		} else {
			playerPosCS.x += offset; 
		}
		owner->TriggerEvent("OnItemTargeted", { true, playerPosCS });
	}
	else {
		if (prevTargetItem) {
			owner->TriggerEvent("OnItemTargeted", { false, Vec2(0.f, 0.f) });
		}
	}

}

void CPlayerController::ChangeControllMode(ControllMode mode)
{
	if (mode == mControllMode) return;
	switch (mControllMode)
	{
	case CPlayerController::ControllMode::None:
		break;
	case CPlayerController::ControllMode::FreeLook:
		break;
	case CPlayerController::ControllMode::LockOn:
		break;
	case CPlayerController::ControllMode::ClassSelection:
		break;
	default:
		break;
	}
	mControllMode = mode;
	if (moveKeyPressed == true) {
		moveKeyPressed = false;
		auto camera = mCamera.lock()->GetTransform();
		Vec3 camForward = camera->GetWorldLook();
		INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
		INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::IDLE);
	}
}

void CPlayerController::LateUpdate()
{
}

void CPlayerController::SetChildAnimationController()
{
	if (auto controller = owner->GetComponentFromHierarchy<CAnimationController>()) {
		auto func = [](float time) {
			INSTANCE(ServerManager).send_cs_attack_packet();
			std::cout << "Do attack packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Attack", "Attack", func);
		controller->AddAnimationEvent("RunAttack", "Attack", func);

		auto func2 = [this](float time) {
			CastingSkill();
			std::cout << "Do skill packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Skill", "Skill", func2);

		auto func3 = [this](float time) {
			if (!mTargetEnemy.lock()) return;
			INSTANCE(ServerManager).send_cs_ultimate_skill_packet();
			std::cout << "Do ultimate skill packet sent!" << std::endl;
			};
		controller->AddAnimationEvent("Ultimate", "Ultimate", func3);
	}
}

void CPlayerController::SetSkill(ITEM_TYPE skill)
{
	mSkill = skill;
	ElementType element = ElementType::end;
	if(skill == ITEM_TYPE::FIRE_ENCHANT || skill == ITEM_TYPE::FIRE_EXPLOSION) {
		element = ElementType::Fire;
	} else if (skill == ITEM_TYPE::WATER_HEAL || skill == ITEM_TYPE::WATER_SHIELD) {
		element = ElementType::Water;
	} else if (skill == ITEM_TYPE::GRASS_WEAKEN || skill == ITEM_TYPE::GRASS_VINE) {
		element = ElementType::Grass;
	} 
	mStateMachine->SetElementType(element);

	owner->TriggerEvent("OnSkillChanged", { (UINT8)skill, (UINT8)element });
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
	case PLAYER_STATE::IDLE: {
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON)) {
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::ATTACK);
			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::SPACE)) {
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::JUMP);
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
		if (INPUT.IsKeyDown(KEY_TYPE::FOUR)) /*임시 씬 전환*/ {
			INSTANCE(ServerManager).send_cs_000_packet(5);
		}
		if (INPUT.IsKeyDown(KEY_TYPE::E)) {
			CastingSkill();
			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::Q)) {
			if (auto item = mTargetItem.lock()) {
				if (item->GetName() == "SkillItem") {
					INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::GATHERING);
				}
				else if (item->GetName() == "Portal") {
					mReadyForNextStage = !mReadyForNextStage; // Toggle ready state for next stage
					INSTANCE(ServerManager).send_cs_ready_for_next_stage_packet(mReadyForNextStage);
				}
			}
		}
		if (INPUT.IsKeyDown(KEY_TYPE::R)) {
			if (mUltimateSkillCooldown < mUltimateSkillCooldownTime) return;
			if (!mTargetEnemy.lock()) return;
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ULTIMATE);
			mUltimateSkillCooldown = 0;
			return;
		}

		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		if (dir != 0) {
			INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::RUN);
			moveKeyPressed = true;
			return;
		}
		break;
	}
	case PLAYER_STATE::RUN: {
		if (INPUT.IsKeyDown(KEY_TYPE::LBUTTON)) {
			//INSTANCE(ServerManager).send_cs_mouse_ldown_packet(camForward);
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::RUNATTACK);

			if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
			if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
			if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
			if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

			if (dir != 0) { INSTANCE(ServerManager).send_cs_move_packet(dir, camForward); }
			return;
		}

		if (INPUT.IsKeyDown(KEY_TYPE::SPACE)) {
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::JUMP);
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
			INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
			return;
		}
		if (INPUT.IsKeyDown(KEY_TYPE::Q)) {
			if (auto item = mTargetItem.lock()) {
				if (item->GetName() == "SkillItem") {
					INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::GATHERING);
					INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
					return;
				}
				else if (item->GetName() == "Portal") {
					mReadyForNextStage = !mReadyForNextStage; // Toggle ready state for next stage
					INSTANCE(ServerManager).send_cs_ready_for_next_stage_packet(mReadyForNextStage);
				}
			}
		}
		if (INPUT.IsKeyDown(KEY_TYPE::R)) {
			if (mUltimateSkillCooldown < mUltimateSkillCooldownTime) return;
			if (!mTargetEnemy.lock()) return;
			mStateMachine->SetState((UINT8)PLAYER_STATE::ULTIMATE);
			INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::ULTIMATE);
			INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
			mUltimateSkillCooldown = 0;
			//mStateMachine->ActivateShield(false);
			return;
		}

		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		if (dir == 0) {
			INSTANCE(ServerManager).send_cs_move_packet(0, camForward);
			INSTANCE(ServerManager).send_cs_change_state_packet((UINT8)PLAYER_STATE::IDLE);
			return;
		}
		INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
		break;
	}
	case PLAYER_STATE::ATTACK:
		break;
	case PLAYER_STATE::RUNATTACK:
		if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
		if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
		if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;

		INSTANCE(ServerManager).send_cs_move_packet(dir, camForward);
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
	if (mSkill == ITEM_TYPE::item_end)
	{
		SetSkill(ITEM_TYPE::item_end);
		return;
	}

	switch (mSkill) {
	case FIRE_ENCHANT:
	case WATER_HEAL:
	case WATER_SHIELD:
	case GRASS_WEAKEN:
		INSTANCE(ServerManager).send_cS_skill_nontarget_packet(mSkill);

		break;
	case FIRE_EXPLOSION: {
		if (mTargetEnemy.lock()) {
			INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, mTargetEnemy.lock()->mID);

		}
		else return;
		break;
	}
	case GRASS_VINE:
		if (mTargetEnemy.lock()) {
			INSTANCE(ServerManager).send_cS_skill_target_packet(mSkill, mTargetEnemy.lock()->mID);
		}
		else return; 
		break;
	}
	SetSkill(ITEM_TYPE::item_end);
	INSTANCE(ServerManager).send_cs_change_state_packet((uint8_t)PLAYER_STATE::SKILL);
}
