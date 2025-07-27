#include "stdafx.h"
#include "UIController.h"
#include "PlayerController.h"
#include"GameObject.h"
#include"Transform.h"
#include"Timer.h"
#include"InputManager.h"
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
#include"ResourceManager.h"
#include"TargetMarker.h"
#include"ParticleAttach.h"
#include"HealthSystem.h"
#include"Button.h"
#include"MentDisplay.h"

CPlayerHUD::CPlayerHUD(const CPlayerHUD& other)
{
    mPlayer = other.mPlayer;
    mOtherPlayers = other.mOtherPlayers;
    mStage = other.mStage;
    mPlayerHUD = nullptr;
	mSettingsUI = nullptr;
}

void CPlayerHUD::Awake()
{
}

void CPlayerHUD::Start()
{
    PLAYER_CLASS playerClass{ PLAYER_CLASS::end };
    ElementType elementType{ ElementType::end };

    mPlayer = INSTANCE(ServerManager).mPlayer;
    for (auto& player : INSTANCE(ServerManager).mOtherPlayers)
    {
        mOtherPlayers.push_back(player.second);
	}

    mPlayerHUD = owner->FindChildByName("PlayerHUD");
	mSettingsUI = owner->FindChildByName("SettingMenuUI");

    
    if (mPlayerHUD) {
        InitializePlayerHUD();
        mPlayerHUD->SetActive(true);
    }
    if (mSettingsUI) {
		InitializeSettingUI();
        mSettingsUI->SetActive(false);
	}
}

void CPlayerHUD::Update()
{
    if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
        if (mUIState == UIState::InGame) {
           INSTANCE(CSceneManager).GetCurScene()->CircularFadeIn(1.f, { 0.0,0.0,0.0,1.f }, []() {
                INSTANCE(CSceneManager).RequestSceneChange(SCENE_TYPE::MENU, false);
                });
        }
        else {
            OnClickResumeButton();
        }
    }
}

void CPlayerHUD::LateUpdate()
{
}

void CPlayerHUD::InitializePlayerHUD()
{
    PLAYER_CLASS playerClass{ PLAYER_CLASS::end };
    ElementType elementType{ ElementType::end };

    auto mainPlayer = mPlayer.lock();
    if (!mainPlayer) return;
    if (auto playerState = mainPlayer->GetComponent<CPlayerStateMachine>())
    {
        playerClass = playerState->GetClass();
        elementType = playerState->GetElementType();
    }

    std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
    std::array<std::string, 4> elementNames = { "Void", "Grass", "Water", "Fire" };

    std::shared_ptr<CUIRenderer> mainPlayerBackgroundRenderer = nullptr;
    std::shared_ptr<CUIRenderer> skillIconRenderer = nullptr;
    std::shared_ptr<CUIRenderer> ultimateBackgroundRenderer = nullptr;

    if (auto mainPlayerUI = owner->FindChildByName("MainPlayer")) {
        if (auto background = mainPlayerUI->FindChildByName("MainPlayer_Background"))
        {
            if (mainPlayerBackgroundRenderer = background->GetComponent<CUIRenderer>())
            {
                mainPlayerBackgroundRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
                mainPlayerBackgroundRenderer->SetTexture("MainPlayer_" + classNames[(UINT8)playerClass] + "_Background");
            }
        }
        if (auto mainPlayerImage = mainPlayerUI->FindChildByName("MainPlayer"))
        {
            if (auto playerRenderer = mainPlayerImage->GetComponent<CUIRenderer>()) {
                playerRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
                playerRenderer->SetTexture("MainPlayer_" + classNames[(UINT8)playerClass]);
            }
        }
        if (auto hpBar = mainPlayerUI->FindChildByName("HealthBar")) {
            auto healthSystem = hpBar->AddComponent<CHealthSystem>();
            healthSystem->BindOwner(mPlayer.lock());
            healthSystem->ViewHealthBar(true);
            healthSystem->SetRenderToWorld(false);
            healthSystem->SetHealthBarColor({ 0.0f, 0.7f, 0.f, 1.f });
        }
    }
    if (auto ultimate = owner->FindChildByName("Ultimate")) {
        if (ultimateBackgroundRenderer = ultimate->GetComponent<CUIRenderer>())
        {
            Color elementColor = { 1.0f, 1.0f, 1.0f, 1.0f };
            if (elementType == ElementType::Void) elementColor = { 1.f, 1.f, 1.f, 1.0f };
            else if (elementType == ElementType::Grass) elementColor = { 0.4196f, 0.9921f, 0.6235f, 1.0f };
            else if (elementType == ElementType::Water) elementColor = { 0.3764f, 0.3372f, 0.9921f, 1.0f };
            else if (elementType == ElementType::Fire) elementColor = { 1.f, 0.27f, 0.2443396f, 1.0f };
            ultimateBackgroundRenderer->SetColor(elementColor);
            ultimateBackgroundRenderer->SetTexture("Ultimate_Background_Void");
        }
        if (auto icon = ultimate->FindChildByName("Ultimate_Icon"))
        {
            if (auto iconRenderer = icon->GetComponent<CUIRenderer>())
            {
                iconRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
                iconRenderer->SetTexture("Ultimate_Icon_" + classNames[(UINT8)playerClass]);
				auto& uiData = iconRenderer->GetUIData();
                uiData.intData0 = 2;

                mainPlayer->AddEvent("OnUltimateSkillCooldown", [iconRenderer](const std::vector<std::any>& args) {
                    if (args.size() < 1) return;
					float ultimateTimeRatio = std::any_cast<float>(args[0]);
                    auto& uiData = iconRenderer->GetUIData();
                    uiData.intData0 = 2;
					uiData.floatData0 = ultimateTimeRatio;
					});
            }
        }
    }
    if (auto skill = owner->FindChildByName("Skill")) {
        if (skillIconRenderer = skill->GetComponent<CUIRenderer>())
        {
            ITEM_TYPE skillType = mPlayer.lock()->GetComponent<CPlayerController>()->GetSkill();
            std::string skillName{ "Background" };
            if (skillType == ITEM_TYPE::FIRE_EXPLOSION) skillName = "Explosion";
            else if (skillType == ITEM_TYPE::GRASS_VINE) skillName = "Vine";
            else if (skillType == ITEM_TYPE::WATER_SHIELD) skillName = "Shield";
            skillIconRenderer->SetTexture("Skill_" + skillName);
        }

    }
    mPlayer.lock()->AddEvent("OnSkillChanged", [mainPlayerBackgroundRenderer, skillIconRenderer, ultimateBackgroundRenderer](const std::vector<std::any>& args) {
        if (args.size() < 2) return;
        if (!mainPlayerBackgroundRenderer) return;
        if (!skillIconRenderer) return;
        if (!ultimateBackgroundRenderer) return;

        UINT8 skillType = std::any_cast<UINT8>(args[0]);
        UINT8 elementType = std::any_cast<UINT8>(args[1]);
        Color elementColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        if (elementType == (UINT)ElementType::Void) elementColor = { 1.f, 1.f, 1.f, 1.0f };
        else if (elementType == (UINT)ElementType::Grass) elementColor = { 0.3608f, 1.f, 0.3922f, 1.0f };
        else if (elementType == (UINT)ElementType::Water) elementColor = { 0.3608f, 0.7961f, 1.f, 1.0f };
        else if (elementType == (UINT)ElementType::Fire) elementColor = { 1.f, 0.4196f, 0.3608f, 1.0f };
        ultimateBackgroundRenderer->SetColor(elementColor);
        mainPlayerBackgroundRenderer->SetColor(elementColor);

        std::string skillName{ "Background" };
        if (skillType == (UINT8)ITEM_TYPE::FIRE_EXPLOSION) skillName = "Explosion";
        else if (skillType == (UINT8)ITEM_TYPE::GRASS_VINE) skillName = "Vine";
        else if (skillType == (UINT8)ITEM_TYPE::WATER_SHIELD) skillName = "Shield";
        skillIconRenderer->SetTexture("Skill_" + skillName);
        }
    );

    if (auto stageUI = owner->GetChildComponent<CUIRenderer>("Title"))
    {
        stageUI->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        stageUI->SetTexture("Title_Stage" + std::to_string(mStage));
    }


    if (auto interactionUI = owner->FindChildByName("InteractionUI")) {
        interactionUI->SetActive(false);
        if (auto renderer = interactionUI->GetComponent<CUIRenderer>())
        {
            renderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }
        mPlayer.lock()->AddEvent("OnItemTargeted", [interactionUI](const std::vector<std::any>& args) {
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

    if (auto targetMarker = owner->FindChildByName("TargetMarker"))
    {
        auto targetMarkerRenderer = targetMarker->GetComponent<CUIRenderer>();
        targetMarkerRenderer->SetTexture("TargetMarker");
        if (auto marker = targetMarker->AddComponent<CTargetMarker>())
        {
            targetMarkerRenderer->mIsVisible = false;
            mPlayer.lock()->AddEvent("OnEnemyTargeted", [marker](const std::vector<std::any>& args) {
                if (args.size() < 1) return;
                auto target = std::any_cast<std::shared_ptr<CGameObject>>(args[0]);
                marker->SetTarget(target);
                });
        }
    }

    if (auto introUI = owner->FindChildByName("IntroUI")) {
        if (auto renderer = introUI->GetComponent<CUIRenderer>())
        {
            renderer->SetAlpha(0.0f);

        }
        auto ment = introUI->AddComponent<CMentDisplay>();
        for (int i = 0; i < 3; i++) {
            std::string texName = "intro" + std::to_string(i + 1);
            ment->AddTexture(texName);
        }
        auto& sm = INSTANCE(ServerManager);
        sm.AddEvent("ShowMent", [introUI, this](std::vector<std::any> any) {
            if (any.size() < 1) return;
            WAVE_TYPE waveType = (WAVE_TYPE)std::any_cast<UINT8>(any[0]);
			if (waveType == WAVE_TYPE::WAVE_END) return;
			auto mentDisplay = introUI->GetComponent<CMentDisplay>();
            if (waveType == WAVE_TYPE::mm || waveType == WAVE_TYPE::mMm) {
				mentDisplay->ClearTextures();
				std::string waveName = "stage" + std::to_string(mStage) + "_" + std::to_string((UINT8)waveType);
                mentDisplay->AddTexture(waveName);
            }
            else if (waveType == WAVE_TYPE::BOSS) {
                mentDisplay->ClearTextures();
                mentDisplay->AddTexture("boss1");
                mentDisplay->AddTexture("boss2");
            }
            else if (waveType == WAVE_TYPE::OUTRO) {
                mentDisplay->ClearTextures();
                mentDisplay->AddTexture("bossEnding1");
                mentDisplay->AddTexture("bossEnding2");
			}
            if (auto renderer = introUI->GetComponent<CUIRenderer>()) {
                renderer->SetAlpha(0.0f);
            }
            introUI->SetActive(true);
            mentDisplay->StartDisplay(4.5f, 1.f, 0.5f);
            });
    }



    for (int i = 0; auto& player : mOtherPlayers)
    {
        BindPlayerToUI(mOtherPlayers[i++].lock(), "Player" + std::to_string(i));
    }
}

void CPlayerHUD::InitializeSettingUI()
{
    if (!mSettingsUI) return;
    if (auto button = mSettingsUI->GetChildComponent<CButton>("ResumeUI"))
    {
        button->SetOnClick([this]() { OnClickResumeButton(); });
	}
    if (auto button = mSettingsUI->GetChildComponent<CButton>("ExitUI"))
    {
        button->SetOnClick([this]() { OnClickResumeButton(); });
    }
}

void CPlayerHUD::ChangeState(UIState newState)
{
    if (mUIState == newState) return;
    switch (newState)
    {
    case UIState::InGame:
        mSettingsUI->SetActive(false);
        break;
    case UIState::Settings:
        mSettingsUI->SetActive(true);
        break;
    default:
        break;
    }
    mUIState = newState;
}

void CPlayerHUD::OnClickResumeButton()
{
	ChangeState(UIState::InGame);
}


void CPlayerHUD::BindPlayerToUI(const std::shared_ptr<class CGameObject>& player, const std::string& name)
{
	if (!player) return;
	if (name.empty()) return;

	PLAYER_CLASS playerClass{ PLAYER_CLASS::end };
    if (auto playerState = player->GetComponent<CPlayerStateMachine>())
    {
        playerClass = playerState->GetClass();
    }

    std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };

    if (auto player = owner->FindChildByName("name"))
    {
        if (auto playerRenderer = player->GetComponent<CUIRenderer>()) {
            playerRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            playerRenderer->SetTexture("Player_" + classNames[(UINT8)playerClass] + "_Normal");
            mOtherPlayers[0].lock()->AddEvent("OnFaceChanged", [playerRenderer, playerClass](const std::vector<std::any>& args) {
                if (args.size() < 1) return;
                bool isNormal = std::any_cast<bool>(args[0]);
                std::array<std::string, 3> classNames = { "Archer", "Fighter", "Mage" };
                std::string faceType = isNormal ? "_Normal" : "_Sad";
                playerRenderer->SetTexture("Player_" + classNames[(UINT8)playerClass] + faceType);
                });
        }
        if (auto hpBar = player->FindChildByName("HealthBar")) {
            auto healthSystem = hpBar->AddComponent<CHealthSystem>();
            healthSystem->BindOwner(player);
            healthSystem->ViewHealthBar(true);
            healthSystem->SetRenderToWorld(false);
            healthSystem->SetHealthBarColor({ 0.0f, 0.8f, 0.f, 1.f });
        }
    }
}
