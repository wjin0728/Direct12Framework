#pragma once
#include"stdafx.h"
#include "Component.h"

class CHealthSystem :
    public CComponent
{
private:
	std::weak_ptr<class CGameObject> mOwner{};

	std::weak_ptr<class CUIRenderer> mHealthBarBackground{};
	std::weak_ptr<class CUIRenderer> mHealthBarEdge{};
	std::weak_ptr<class CUIRenderer> mHealthBarFill{};

	Vec3 mHealthBarPosition = Vec3(0.0f, 0.0f, 0.0f);
	Vec2 mHealthBarScale = Vec2(1.0f, 1.0f); 

	float mHealth = 10.0f;
	float mMaxHealth = 10.0f;
	float mPrevHealth = 0;

	bool mViewHealthBar = true;
	bool mRenderToWorld = false;
	Color mHealthBarColor = Color(0.0f, 1.0f, 0.0f, 1.0f); 

public:
	CHealthSystem();
    virtual ~CHealthSystem();
	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CHealthSystem>(*this); }

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void SetHealth(float health) { mPrevHealth = health; mHealth = health;  UpdateHealthBar(); }
	void SetMaxHealth(float maxHealth)
	{ 
		mMaxHealth = maxHealth; 
		if (mHealth > mMaxHealth) mHealth = mMaxHealth;
		UpdateHealthBar(); 
	}

	void GetDamage(float damage) 
	{ 
		if(mPrevHealth <= mHealth) mPrevHealth = mHealth;
		mHealth -= damage; 
		if (mHealth < 0.0f) {
			mHealth = 0.0f;
		}
		UpdateHealthBar(); 
	}
	void Heal(float healAmount) 
	{ 
		mPrevHealth = mHealth;
		mHealth += healAmount; 
		if (mHealth > mMaxHealth) mHealth = mMaxHealth; 
		UpdateHealthBar(); 
	}

	void ChangeHealth(float healthChange) 
	{ 
		if (mPrevHealth <= mHealth) mPrevHealth = mHealth;
		mHealth = healthChange;
		mHealth = std::clamp(mHealth, 0.0f, mMaxHealth);
		UpdateHealthBar();
	}

	void UpdateHealthBar();
	void ViewHealthBar(bool view);
	void SetRenderToWorld(bool renderToWorld) { mRenderToWorld = renderToWorld; UpdateHealthBar(); }

	void SetHealthBarPosition(const Vec3& position) { mHealthBarPosition = position; UpdateHealthBar(); }
	void SetBackgroundTexture(const std::string& textureName);
	void SetEdgeTexture(const std::string& textureName);
	void SetFillTexture(const std::string& textureName);
	void SetHealthBarColor(const Color& color) { mHealthBarColor = color; UpdateHealthBar(); }
	void SetHealthBarScale(const Vec2& size);

	void BindOwner(const std::shared_ptr<class CGameObject>& owner);
};

