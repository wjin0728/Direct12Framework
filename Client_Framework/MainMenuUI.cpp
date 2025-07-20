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
                INSTANCE(CSceneManager).RequestSceneChange(SCENE_TYPE::LOADING, false);
				};

            button->SetOnClick([fadeFunction]() {
				fadeFunction();
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
