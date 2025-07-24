#pragma once
#include "stdafx.h"
#include "Component.h"

class CMainMenu :
    public CComponent
{
public:
	enum class EMenuState
	{
		Title,
		Settings,
		RoomSelect
	};
private:
	EMenuState mCurrentState = EMenuState::Title;
	std::shared_ptr<CGameObject> mTitleUI;
	std::shared_ptr<CGameObject> mSettingsUI;
	std::shared_ptr<CGameObject> mRoomSelectUI;

public:
	CMainMenu();
	virtual ~CMainMenu() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMainMenu>(*this); }

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

	void InitializeTitleUI();
	void InitializeSettingsUI();
	void InitializeRoomSelectUI();

	void ChangeMenuState(EMenuState newState);
	void OnClickStartButton();
	void OnClickExitButton();

	void OnClickSettingsButton();

	void OnClickBackButton();

};

