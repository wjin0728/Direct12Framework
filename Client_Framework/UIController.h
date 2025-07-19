#pragma once
#include "stdafx.h"
#include "Component.h"

class CPlayerHUD :
    public CComponent
{
public:
	std::weak_ptr<class CGameObject> mPlayer;
	std::array<std::weak_ptr<class CGameObject>, 2> mOtherPlayers{};

	std::unordered_map<std::string, std::weak_ptr<class CUIRenderer>> mUIRenderers{};

public:
	CPlayerHUD() = default;
	~CPlayerHUD() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CPlayerHUD>(*this); }

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

public:
	void BindPlayerToUI(const std::shared_ptr<CGameObject>& player, const std::string& name);
};

