#include "stdafx.h"
#include "ClassSelect.h"
#include "PlayerController.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"InputManager.h"
#include"SceneManager.h"
#include"Scene.h"
#include"Camera.h"
#include"RenderManager.h"
#include"UIRenderer.h"
#include"ResourceManager.h"
#include"Button.h"
#include"ServerManager.h"
#include "UIController.h"
#include"ServerManager.h"
#include "ObjectState.h"
#include "ThirdPersonCamera.h"


CClassSelectUI::CClassSelectUI()
{
}

CClassSelectUI::CClassSelectUI(const CClassSelectUI& other)
{
    mCurrentState = other.mCurrentState;
	mSettingsUI = nullptr;
	mWaitingRoomUI = nullptr;
	mClassSelectUI = nullptr;
}

void CClassSelectUI::Awake()
{
}

void CClassSelectUI::Start()
{
    mCurrentState = EMenuState::ClassSelect;
	mClassSelectUI = owner->FindChildByName("ClassSelectUI");
	mSettingsUI = owner->FindChildByName("SettingsUI");
	mWaitingRoomUI = owner->FindChildByName("WaitingRoomUI");
    InitializeClassSelectUI();
    InitializeSettingsUI();
    InitializeWaitingRoomUI();

	mCamera = INSTANCE(CRenderManager).GetCamera("MainCamera");

	mClassCharacters[0] = INSTANCE(CSceneManager).GetCurScene()->FindObjectWithName("Archer");
	mClassCharacters[1] = INSTANCE(CSceneManager).GetCurScene()->FindObjectWithName("Fighter");
	mClassCharacters[2] = INSTANCE(CSceneManager).GetCurScene()->FindObjectWithName("Mage");

	mWaitingRoomUI->SetActive(false);

	auto& sm = INSTANCE(ServerManager);
	sm.AddEvent("SelectClass", [this](std::vector<std::any> any) {
		auto player = INSTANCE(ServerManager).mPlayer;
		auto& otherPlayers = INSTANCE(ServerManager).mOtherPlayers;
		if (!player) return;
		if (any.size() < 1) return;

		uint8_t classType = std::any_cast<uint8_t>(any[0]);
		Vec3 position = mClassCharacters[classType]->GetTransform()->GetLocalPosition();
		Vec3 rotation = mClassCharacters[classType]->GetTransform()->GetLocalRotation();
		Vec3 scale = mClassCharacters[classType]->GetTransform()->GetLocalScale();


		short playerId = std::any_cast<short>(any[1]);
		std::shared_ptr<CGameObject> playerObj{};
		if (playerId == INSTANCE(ServerManager).clientID) {
			playerObj = player;
		}
		else {
			playerObj = otherPlayers[playerId];
			mSelectedClasses.push_back((PLAYER_CLASS)classType);
		}

		std::cout << "Selected class: " << classType << endl;

		playerObj->GetTransform()->SetLocalPosition(position);
		playerObj->GetTransform()->SetLocalRotation(rotation);
		playerObj->GetTransform()->SetLocalScale(scale);

		mClassCharacters[classType]->GetTransform()->SetLocalPosition({ 0.f, 0.f, 0.f });
		mClassCharacters[classType]->GetTransform()->SetLocalRotation({ 0.f, 0.f, 0.f });
		mClassCharacters[classType]->GetTransform()->SetLocalScale({ 1.f, 1.f, 1.f });
		mClassCharacters[classType]->GetTransform()->SetParentInScene(playerObj->GetTransform());

		std::shared_ptr<CPlayerStateMachine> stateMachine{};
		if (classType == (UINT8)PLAYER_CLASS::ARCHER) {
			stateMachine = playerObj->AddComponent<CArcherState>();
		}
		else if (classType == (UINT8)PLAYER_CLASS::FIGHTER) {
			stateMachine = playerObj->AddComponent<CWarriorState>();
		}
		else if (classType == (UINT8)PLAYER_CLASS::MAGE) {
			stateMachine = playerObj->AddComponent<CMageState>();
		}

		stateMachine->SetState((UINT8)PLAYER_STATE::IDLE);
		playerObj->SetStateMachine(stateMachine);

		auto shieldPrefab = RESOURCE.GetPrefab("Water_Shield");
		if (shieldPrefab) {
			auto shieldObj = CGameObject::Instantiate(shieldPrefab, playerObj->GetTransform());
			shieldObj->SetRenderLayer(RENDER_LAYER::Transparent);
			shieldObj->GetTransform()->SetLocalPosition({ 0.f, 0.6f, 0.f });
			stateMachine->SetShield(shieldObj);
			stateMachine->ActivateShield(false);
		}

		if (playerId == INSTANCE(ServerManager).clientID) {
			auto playerController = playerObj->GetComponent<CPlayerController>();
			if (playerController) {
				playerController->ChangeControllMode(CPlayerController::ControllMode::LockOn);
				playerController->SetStateMachine(stateMachine);
				playerController->SetChildAnimationController();
			}
			if (auto camera = INSTANCE(CSceneManager).GetCurScene()->FindObjectWithName("MainCamera")) {
				if (auto cameraCmp = camera->GetComponent<CCamera>()) {
					cameraCmp->GenerateReverseZPerspectiveProjectionMatrix(0.1f, 150.f, 60.f);
					cameraCmp->SetCameraType(CCamera::CameraType::Perspective);
				}
				auto thirdPersonCamera = camera->GetComponent<CThirdPersonCamera>();
				if (thirdPersonCamera) {
					thirdPersonCamera->SetDefaultCameraParams();
					thirdPersonCamera->ChangeCameraMode(CThirdPersonCamera::CameraMode::FollowTarget);
				}
			}
		}

		std::string buttonName = "SelectBound" + std::to_string(classType + 1);
		if (auto button = mClassSelectUI->FindChildByName(buttonName)) {
			button->SetActive(false);
		}

		if (mCurrentState == EMenuState::WaitingRoom) {
			std::string playerName = "Player" + std::to_string(mSelectedClasses.size());
			if (auto player1 = owner->GetChildComponent<CUIRenderer>(playerName)) {
				auto& serverManager = INSTANCE(ServerManager);
				std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
				if (any.size() < 1) return;
				if (player1) {
					player1->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
					player1->SetTexture("Player_" + classNames[classType] + "_Normal");
				}
			}
		}
		}
	);
}

void CClassSelectUI::Update()
{
}

void CClassSelectUI::LateUpdate()
{
}

void CClassSelectUI::ChangeMenuState(EMenuState newState)
{
    if (mCurrentState == newState) return;
    switch (newState)
    {
    case EMenuState::ClassSelect:
		mClassSelectUI->SetActive(true);
		mWaitingRoomUI->SetActive(false);
        break;
    case EMenuState::Setting:
		mClassSelectUI->SetActive(false);
		mWaitingRoomUI->SetActive(false);
        break;
    case EMenuState::WaitingRoom:
		mClassSelectUI->SetActive(false);
		mWaitingRoomUI->SetActive(true);
		for (int i = 0; i < mSelectedClasses.size();i++) {
			std::string playerName = "Player" + std::to_string(mSelectedClasses.size());
			if (auto player1 = owner->GetChildComponent<CUIRenderer>(playerName)) {
				auto& serverManager = INSTANCE(ServerManager);
				std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
				if (player1) {
					player1->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
					player1->SetTexture("Player_" + classNames[i] + "_Normal");
				}
			}
		}
        break;
    default:
        break;
    }
	mCurrentState = newState;
}


void CClassSelectUI::InitializeClassSelectUI()
{
	if (auto button = mClassSelectUI->GetChildComponent<CButton>("SelectBound1"))
	{
		button->SetOnClick([this]() { OnClickSelectButton(0); });
	}
	if (auto button = mClassSelectUI->GetChildComponent<CButton>("SelectBound2"))
	{
		button->SetOnClick([this]() { OnClickSelectButton(1); });
	}
	if (auto button = mClassSelectUI->GetChildComponent<CButton>("SelectBound3"))
	{
		button->SetOnClick([this]() { OnClickSelectButton(2); });
	}
}

void CClassSelectUI::InitializeSettingsUI()
{

}

void CClassSelectUI::InitializeWaitingRoomUI()
{
	if (auto player1 = owner->GetChildComponent<CUIRenderer>("Player1")) {
		auto& serverManager = INSTANCE(ServerManager);
		serverManager.AddEvent("Player1Joined", [player1](std::vector<std::any> any) {
			std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
			if (any.size() < 1) return;
			int playerClass = std::any_cast<int>(any[0]);
			if (player1) {
				player1->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				player1->SetTexture("Player_" + classNames[playerClass] + "_Normal");
			}
			}
		);
	}
	if (auto player2 = owner->GetChildComponent<CUIRenderer>("Player2")) {
		auto& serverManager = INSTANCE(ServerManager);
		serverManager.AddEvent("Player2Joined", [player2](std::vector<std::any> any) {
			std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
			if (any.size() < 1) return;
			int playerClass = std::any_cast<int>(any[0]);
			if (player2) {
				player2->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				player2->SetTexture("Player_" + classNames[playerClass] + "_Normal");
			}
			}
		);
	}
}


void CClassSelectUI::OnClickExitButton()
{
	
}

void CClassSelectUI::OnClickSelectButton(int classType)
{
	INSTANCE(ServerManager).send_cs_click_button_packet(classType);
	ChangeMenuState(EMenuState::WaitingRoom);

	INPUT.FixMousePosition(true);
}

void CClassSelectUI::OnClickSettingsButton()
{
	ChangeMenuState(EMenuState::Setting);
}