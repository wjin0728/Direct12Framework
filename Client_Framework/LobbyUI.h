#pragma once
#include "stdafx.h"
#include "Component.h"

class CLobbyMenu :
    public CComponent
{
public:
	std::unordered_map<std::string, std::weak_ptr<class CUIRenderer>> mUIRenderers{};

public:
	CLobbyMenu();
	virtual ~CLobbyMenu() = default;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CLobbyMenu>(*this); }

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

};

