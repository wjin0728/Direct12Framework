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
#include "MentDisplay.h"


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
	auto introUI = mWaitingRoomUI->FindChildByName("IntroUI");
	introUI->SetActive(false);

	auto& otherPlayers = INSTANCE(ServerManager).mOtherPlayers;
	for (auto& player : otherPlayers) {
		if (player.second) {
			auto playerState = std::dynamic_pointer_cast<CPlayerStateMachine>(player.second->GetStateMachine());
			uint8_t classType = (uint8_t)playerState->GetClass();
			Vec3 position = mClassCharacters[classType]->GetTransform()->GetLocalPosition();
			Quaternion rotation = mClassCharacters[classType]->GetTransform()->GetLocalRotation();
			Vec3 scale = mClassCharacters[classType]->GetTransform()->GetLocalScale();

			mSelectedClasses.push_back((PLAYER_CLASS)classType);

			std::cout << "Selected class: " << classType << endl;

			player.second->GetTransform()->SetLocalPosition(position);
			player.second->GetTransform()->SetLocalRotation(rotation);
			player.second->GetTransform()->SetLocalScale(scale);

			mClassCharacters[classType]->GetTransform()->SetLocalPosition({ 0.f, 0.f, 0.f });
			mClassCharacters[classType]->GetTransform()->SetLocalRotation({ 0.f, 0.f, 0.f });
			mClassCharacters[classType]->GetTransform()->SetLocalScale({ 1.f, 1.f, 1.f });
			mClassCharacters[classType]->GetTransform()->SetParentInScene(player.second->GetTransform());


			playerState->SetState((UINT8)PLAYER_STATE::IDLE);
			playerState->CreateParticleEvent();

			auto shieldPrefab = RESOURCE.GetPrefab("Water_Shield");
			if (shieldPrefab) {
				auto shieldObj = CGameObject::Instantiate(shieldPrefab, player.second->GetTransform());
				shieldObj->SetRenderLayer(RENDER_LAYER::Transparent);
				shieldObj->GetTransform()->SetLocalPosition({ 0.f, 0.6f, 0.f });
				playerState->SetShield(shieldObj);
				playerState->ActivateShield(false);
			}

			std::string buttonName = "SelectBound" + std::to_string(classType + 1);
			if (auto button = mClassSelectUI->FindChildByName(buttonName)) {
				button->SetActive(false);
			}
		}
	}

	auto& sm = INSTANCE(ServerManager);
	sm.AddEvent("SelectClass", [this](std::vector<std::any> any) {
		auto player = INSTANCE(ServerManager).mPlayer;
		auto& otherPlayers = INSTANCE(ServerManager).mOtherPlayers;
		if (!player) return;
		if (any.size() < 1) return;

		uint8_t classType = std::any_cast<uint8_t>(any[0]);
		Vec3 position = mClassCharacters[classType]->GetTransform()->GetLocalPosition();
		Quaternion rotation = mClassCharacters[classType]->GetTransform()->GetLocalRotation();
		Vec3 scale = mClassCharacters[classType]->GetTransform()->GetLocalScale();

		short playerId = std::any_cast<short>(any[1]);
		std::shared_ptr<CGameObject> playerObj{};
		if (playerId == INSTANCE(ServerManager).clientID) {
			playerObj = player;
		}
		else {
			playerObj = otherPlayers[playerId];
			mSelectedClasses.push_back((PLAYER_CLASS)classType);
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

		std::cout << "Selected class: " << classType << endl;

		playerObj->GetTransform()->SetLocalPosition(position);
		playerObj->GetTransform()->SetLocalRotation(rotation);
		playerObj->GetTransform()->SetLocalScale(scale);

		mClassCharacters[classType]->GetTransform()->SetLocalPosition({ 0.f, 0.f, 0.f });
		mClassCharacters[classType]->GetTransform()->SetLocalRotation({ 0.f, 0.f, 0.f });
		mClassCharacters[classType]->GetTransform()->SetLocalScale({ 1.f, 1.f, 1.f });
		mClassCharacters[classType]->GetTransform()->SetParentInScene(playerObj->GetTransform());

		
		auto playerState = std::dynamic_pointer_cast<CPlayerStateMachine>(playerObj->GetStateMachine());
		playerState->CreateParticleEvent();
		playerState->SetState((UINT8)PLAYER_STATE::IDLE);

		auto shieldPrefab = RESOURCE.GetPrefab("Water_Shield");
		if (shieldPrefab) {
			auto shieldObj = CGameObject::Instantiate(shieldPrefab, playerObj->GetTransform());
			shieldObj->SetRenderLayer(RENDER_LAYER::Transparent);
			shieldObj->GetTransform()->SetLocalPosition({ 0.f, 0.6f, 0.f });
			playerState->SetShield(shieldObj);
			playerState->ActivateShield(false);
		}

		if (playerId == INSTANCE(ServerManager).clientID) {
			auto playerController = playerObj->GetComponent<CPlayerController>();
			if (playerController) {
				playerController->ChangeControllMode(CPlayerController::ControllMode::LockOn);
				playerController->SetStateMachine(playerState);
				playerController->SetChildAnimationController();
			}
			if (auto camera = INSTANCE(CSceneManager).GetCurScene()->FindObjectWithName("MainCamera")) {
				if (auto cameraCmp = camera->GetComponent<CCamera>()) {
					cameraCmp->GenerateReverseZPerspectiveProjectionMatrix(0.1f, 150.f, 60.f);
					cameraCmp->SetCameraType(CCamera::CameraType::Perspective);
				}
				auto thirdPersonCamera = camera->GetComponent<CThirdPersonCamera>();
				if (thirdPersonCamera) {
					//thirdPersonCamera->SetDefaultCameraParams();
					thirdPersonCamera->ChangeCameraMode(CThirdPersonCamera::CameraMode::FollowTarget);
				}
			}
		}

		std::string buttonName = "SelectBound" + std::to_string(classType + 1);
		if (auto button = mClassSelectUI->FindChildByName(buttonName)) {
			button->SetActive(false);
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
		{
			for (int i = 0; i < mSelectedClasses.size(); i++) {
				std::string playerName = "Player" + std::to_string(mSelectedClasses.size());
				if (auto player1 = owner->GetChildComponent<CUIRenderer>(playerName)) {
					auto& serverManager = INSTANCE(ServerManager);
					std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
					if (player1) {
						player1->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
						player1->SetTexture("Player_" + classNames[(int)mSelectedClasses[i]] + "_Normal");
					}
				}
			}
			auto introUI = mWaitingRoomUI->FindChildByName("IntroUI");
			introUI->SetActive(false);
			break;
		}
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
	auto& sm = INSTANCE(ServerManager);
	if (auto introUI = mWaitingRoomUI->FindChildByName("IntroUI")) {
		if(auto renderer = introUI->GetComponent<CUIRenderer>())
		{
			renderer->SetAlpha(0.0f);

		}
		auto ment = introUI->AddComponent<CMentDisplay>();
		for(int i = 0; i < 3; i++) {
			std::string texName = "intro" + std::to_string(i + 1);
			ment->AddTexture(texName);
		}
		std::weak_ptr<CGameObject> introUIWeak = introUI;
		sm.AddEvent("ShowMent", [introUIWeak](std::vector<std::any> any) {
			if (introUIWeak.expired()) return;
			auto introUI = introUIWeak.lock();
			if (!introUI) return;
			if (any.size() < 1) return;
			WAVE_TYPE waveType = (WAVE_TYPE)std::any_cast<UINT8>(any[0]);
			if (auto renderer = introUI->GetComponent<CUIRenderer>()) {
				renderer->SetAlpha(0.0f);
			}
			introUI->SetActive(true);

			introUI->GetComponent<CMentDisplay>()->StartDisplay(6.f, 1.5f, 0.6f);
			});
	}

	if (auto interactionUI = owner->FindChildByName("InteractionUI")) {
		interactionUI->SetActive(false);
		if (auto renderer = interactionUI->GetComponent<CUIRenderer>())
		{
			renderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}
		if(sm.mPlayer)
			sm.mPlayer->AddEvent("OnItemTargeted", [interactionUI](const std::vector<std::any>& args) {
			if (args.size() < 1) return;
			bool isActive = std::any_cast<bool>(args[0]);
			interactionUI->SetActive(isActive);
			if (isActive)
			{
				if (auto renderer = interactionUI->GetComponent<CUIRenderer>())
				{
					Vec2 screenPos = std::any_cast<Vec2>(args[1]);
					renderer->SetPosition(screenPos);
				}
			}
			});
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