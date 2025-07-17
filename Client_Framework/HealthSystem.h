#pragma once
#include"stdafx.h"
#include "Component.h"

class CHealthSystem :
    public CComponent
{
private:
	std::weak_ptr<class CUIRenderer> mHealthBarBackground{};
	std::weak_ptr<class CUIRenderer> mHealthBarEdge{};
	std::weak_ptr<class CUIRenderer> mHealthBarFill{};

	Vec3 mHealthBarPosition = Vec3(0.0f, 0.0f, 0.0f);

	float mHealth = 1.0f;
	float mMaxHealth = 10.0f;

public:
	CHealthSystem();
    virtual ~CHealthSystem();
	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CHealthSystem>(*this); }

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void SetHealth(float health) { mHealth = health; UpdateHealthBar(); }
	void SetMaxHealth(float maxHealth) { mMaxHealth = maxHealth; UpdateHealthBar(); }

	void GetDamage(float damage) 
	{ 
		mHealth -= damage; 
		if (mHealth < 0.0f) mHealth = 0.0f; 
		UpdateHealthBar(); 
	}
	void Heal(float healAmount) 
	{ 
		mHealth += healAmount; 
		if (mHealth > mMaxHealth) mHealth = mMaxHealth; 
		UpdateHealthBar(); 
	}

	void UpdateHealthBar();

};

