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
    if (auto startButtonObj = owner->FindChildByName("NewGame")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto fadeFunction = []() {
                INSTANCE(ServerManager).send_cs_click_button_packet((uint8_t)BUTTON_TYPE::MAIN_UI_GAME_START);

                INSTANCE(CSceneManager).RequestSceneChange(SCENE_TYPE::LOADING, false);
			};

            button->SetOnClick([fadeFunction]() {
                INSTANCE(CSceneManager).GetCurScene()->FadeIn(0.5f, {0.f,0.f,0.f, 0.f}, fadeFunction);
            });
		}
    }
    if (auto exitButtonObj = owner->FindChildByName("Exit")) {
        if (auto button = exitButtonObj->GetComponent<CButton>())
        {
            auto exitFunction = []() {
                ::PostQuitMessage(0);
            };
            button->SetOnClick(exitFunction);
        }
    }
}

void CMainMenu::Update()
{

}

void CMainMenu::LateUpdate()
{
}
