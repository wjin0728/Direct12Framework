#pragma once
#include"stdafx.h"
#include "Component.h"


class CButton :
    public CComponent
{
public:
    std::weak_ptr<class CGameObject> mTarget{};
	std::weak_ptr<class CUIRenderer> mTargetRenderer{};
    std::string mTargetFunctionName{};
	Vec2 mTargetSize{};
	Vec2 mTargetPos{};
    bool mIsPressed = false;
	bool mIsHovered = false;
    bool mIsReleased = false;
	std::function<void()> mOnClick = nullptr;

public:
    CButton() {}
    virtual ~CButton() {}

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CButton>(*this); }

public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
	virtual void LateUpdate() override;

	void SetOnClick(std::function<void()> func) { mOnClick = func; }

};

