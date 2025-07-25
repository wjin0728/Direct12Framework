#pragma once
#include "stdafx.h"
#include "Component.h"

class CClassSelectUI :
    public CComponent
{
public:
	enum class EMenuState
	{
		ClassSelect,
		Setting,
		WaitingRoom
	};
private:
	EMenuState mCurrentState = EMenuState::ClassSelect;
	std::shared_ptr<CGameObject> mClassSelectUI = nullptr;
	std::shared_ptr<CGameObject> mSettingsUI = nullptr;
	std::shared_ptr<CGameObject> mWaitingRoomUI = nullptr;

	std::array<std::shared_ptr<CGameObject>, 3> mClassChoices = { nullptr, nullptr, nullptr };
	std::array<std::shared_ptr<CGameObject>, 3> mClassCharacters = { nullptr, nullptr, nullptr };

	CCamera* mCamera = nullptr;

public:
	CClassSelectUI();
	CClassSelectUI(const CClassSelectUI& other);
	virtual ~CClassSelectUI() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CClassSelectUI>(*this); }

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

	void InitializeClassSelectUI();
	void InitializeSettingsUI();
	void InitializeWaitingRoomUI();

	void ChangeMenuState(EMenuState newState);
	void OnClickExitButton();
	void OnClickSelectButton(int classType);

	void OnClickSettingsButton();

};

