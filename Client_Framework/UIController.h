#pragma once
#include "stdafx.h"
#include "Component.h"

class CPlayerHUD :
    public CComponent
{
public:
	enum class UIState
	{
		InGame,
		Settings
	};
	UIState mUIState = UIState::InGame;

	std::weak_ptr<class CGameObject> mPlayer;
	std::vector<std::weak_ptr<class CGameObject>> mOtherPlayers{};
	int mStage = 0;

	std::shared_ptr<class CGameObject> mPlayerHUD;
	std::shared_ptr<class CGameObject> mSettingsUI;


public:
	CPlayerHUD() = default;
	CPlayerHUD(const CPlayerHUD& other);
	CPlayerHUD(int stage) : mStage(stage) {}
	~CPlayerHUD() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CPlayerHUD>(*this); }

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

public:
	void InitializePlayerHUD();
	void InitializeSettingUI();

	void ChangeState(UIState newState);
	void OnClickResumeButton();
	void OnClickExitButton();


};

