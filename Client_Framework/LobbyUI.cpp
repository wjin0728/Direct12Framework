#include "stdafx.h"
#include "LobbyUI.h"
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

CLobbyMenu::CLobbyMenu()
{
}

void CLobbyMenu::Awake()
{
}

void CLobbyMenu::Start()
{
    if (auto startButtonObj = owner->FindChildByName("Room1")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
				//버튼 클릭시 실행되는 함수
			};

            button->SetOnClick(func);
		}
    }
    if (auto startButtonObj = owner->FindChildByName("Room2")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
                //버튼 클릭시 실행되는 함수
                };

            button->SetOnClick(func);
        }
    }
    if (auto startButtonObj = owner->FindChildByName("Room3")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
                //버튼 클릭시 실행되는 함수
                };

            button->SetOnClick(func);
        }
    }
    if (auto startButtonObj = owner->FindChildByName("Room4")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
                //버튼 클릭시 실행되는 함수
                };

            button->SetOnClick(func);
        }
    }
    if (auto startButtonObj = owner->FindChildByName("Room5")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
                //버튼 클릭시 실행되는 함수
                };

            button->SetOnClick(func);
        }
    }
    if (auto startButtonObj = owner->FindChildByName("Room6")) {
        if (auto button = startButtonObj->GetComponent<CButton>())
        {
            auto func = []() {
                //버튼 클릭시 실행되는 함수
                };

            button->SetOnClick(func);
        }
    }
}

void CLobbyMenu::Update()
{

}

void CLobbyMenu::LateUpdate()
{
}
