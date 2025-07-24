#include "stdafx.h"
#include "MainMenuUI.h"
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

CMainMenu::CMainMenu()
{
}

void CMainMenu::Awake()
{
}

void CMainMenu::Start()
{
	mTitleUI = owner->FindChildByName("TitleUI");
    mSettingsUI = owner->FindChildByName("SettingUI");
    mRoomSelectUI = owner->FindChildByName("RoomSelectUI");
    if (mTitleUI) {
        InitializeTitleUI();
    }
    if (mSettingsUI) {
        InitializeSettingsUI();
    }
    if (mRoomSelectUI) {
        InitializeRoomSelectUI();
    }

    mTitleUI->SetActive(true);
    mSettingsUI->SetActive(false);
    mRoomSelectUI->SetActive(false);
}

void CMainMenu::Update()
{
    if (INPUT.IsKeyDown(KEY_TYPE::ESCAPE)) {
        if (mCurrentState == EMenuState::Title) {
            OnClickExitButton();
        } else {
            OnClickBackButton();
		}
    }
}

void CMainMenu::LateUpdate()
{
}

void CMainMenu::ChangeMenuState(EMenuState newState)
{
    if (mCurrentState == newState) return;
    switch (newState)
    {
    case EMenuState::Title:
        mTitleUI->SetActive(true);
        mSettingsUI->SetActive(false);
        mRoomSelectUI->SetActive(false);
        break;
    case EMenuState::Settings:
        mTitleUI->SetActive(false);
        mSettingsUI->SetActive(true);
        mRoomSelectUI->SetActive(false);
        break;
    case EMenuState::RoomSelect:
        mTitleUI->SetActive(false);
        mSettingsUI->SetActive(false);
        mRoomSelectUI->SetActive(true);
        break;
    default:
        break;
    }
	mCurrentState = newState;
}

void CMainMenu::InitializeTitleUI()
{
    if (auto button = mTitleUI->GetChildComponent<CButton>("NewGame"))
    {
        button->SetOnClick([this]() { OnClickStartButton(); });
    }
    if (auto button = mTitleUI->GetChildComponent<CButton>("Settings"))
    {
        button->SetOnClick([this]() { OnClickSettingsButton(); });
	}
    if (auto button = mTitleUI->GetChildComponent<CButton>("Exit"))
    {
        button->SetOnClick([this]() { OnClickExitButton(); });
	}
}

void CMainMenu::InitializeSettingsUI()
{

}

void CMainMenu::InitializeRoomSelectUI()
{
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room1"))
    {
        button->SetOnClick([this]() { /*클릭 시 실행될 코드*/ });
    }
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room2"))
    {
        button->SetOnClick([this]() {  /*클릭 시 실행될 코드*/ });
	}
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room3"))
    {
        button->SetOnClick([this]() {  /*클릭 시 실행될 코드*/ });
    }
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room4"))
    {
        button->SetOnClick([this]() {  /*클릭 시 실행될 코드*/ });
	}
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room5"))
    {
        button->SetOnClick([this]() {  /*클릭 시 실행될 코드*/ });
    }
    if (auto button = mRoomSelectUI->GetChildComponent<CButton>("Room6"))
    {
        button->SetOnClick([this]() {  /*클릭 시 실행될 코드*/ });
    }
}


void CMainMenu::OnClickStartButton()
{
    //INSTANCE(ServerManager).send_cs_click_button_packet((uint8_t)BUTTON_TYPE::MAIN_UI_GAME_START);
    ChangeMenuState(EMenuState::RoomSelect);
}

void CMainMenu::OnClickExitButton()
{
	::PostQuitMessage(0);
}

void CMainMenu::OnClickSettingsButton()
{
	ChangeMenuState(EMenuState::Settings);
}

void CMainMenu::OnClickBackButton()
{
    ChangeMenuState(EMenuState::Title);
}