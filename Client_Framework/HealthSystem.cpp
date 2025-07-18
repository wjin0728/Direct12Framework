#include "stdafx.h"
#include "TargetMarker.h"
#include "GameObject.h"
#include "Transform.h"
#include"UIRenderer.h"
#include"Camera.h"
#include "RenderManager.h"
#include"Timer.h"
#include "HealthSystem.h"
#include"Timer.h"


CHealthSystem::CHealthSystem()
{
}

CHealthSystem::~CHealthSystem()
{
}

void CHealthSystem::Awake()
{

}

void CHealthSystem::Start()
{
	mHealthBarBackground = owner->GetComponent<CUIRenderer>();
	mHealthBarFill = owner->FindChildByName("HP_Fill")->GetComponent<CUIRenderer>();
	mHealthBarEdge = owner->FindChildByName("HP_Edge")->GetComponent<CUIRenderer>();

	auto fill = mHealthBarFill.lock();
	auto background = mHealthBarBackground.lock();
	auto edge = mHealthBarEdge.lock();
	if (fill)
	{
		CBUIData& uiData = fill->GetUIData();
		uiData.intData0 = 1;
		fill->SetShader("Sprite");
		fill->SetColor(mHealthBarColor);
		fill->SetScale(mHealthBarScale);
	}
	if (background)
	{
		background->SetShader("Sprite");
		background->SetScale(mHealthBarScale);
	}
	if (edge)
	{
		edge->SetShader("Sprite");
		edge->SetScale(mHealthBarScale);
	}
	UpdateHealthBar();
}

void CHealthSystem::Update()
{
}

void CHealthSystem::LateUpdate()
{
	if(mRenderToWorld) {
		auto background = mHealthBarBackground.lock();
		auto fill = mHealthBarFill.lock();
		auto edge = mHealthBarEdge.lock();

		auto camera = INSTANCE(CRenderManager).GetCamera("MainCamera");
		if (!camera) {
			return;
		}
		Vec2 screenPos = camera->TransformToNDC(GetTransform()->GetWorldPosition());
		if (background) {
			background->SetPosition(screenPos);
		}
		if (fill) {
			fill->SetPosition(screenPos);
		}
		if (edge) {
			edge->SetPosition(screenPos);
		}
	}
	mPrevHealth = lerp(mPrevHealth, mHealth, 5.f * DELTA_TIME);
	UpdateHealthBar();
}

void CHealthSystem::UpdateHealthBar()
{
	float hpRatio = mHealth / mMaxHealth;
	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f);
	auto fill = mHealthBarFill.lock();
	if (fill) 
	{
		CBUIData& uiData = fill->GetUIData();
		uiData.floatData0 = hpRatio;
		uiData.floatData1 = std::clamp(mPrevHealth / mMaxHealth, 0.0f, 1.0f);
	}
}

void CHealthSystem::ViewHealthBar(bool view)
{
	auto background = mHealthBarBackground.lock();
	auto fill = mHealthBarFill.lock();
	auto edge = mHealthBarEdge.lock();
	if (background) background->mIsVisible = view;
	if (fill) fill->mIsVisible = view;
	if (edge) edge->mIsVisible = view;
	UpdateHealthBar();
}

void CHealthSystem::SetBackgroundTexture(const std::string& textureName)
{
	auto background = mHealthBarBackground.lock();
	if(background) background->SetTexture(textureName);
}

void CHealthSystem::SetEdgeTexture(const std::string& textureName)
{
	auto edge = mHealthBarEdge.lock();
	if(edge) edge->SetTexture(textureName);
}

void CHealthSystem::SetFillTexture(const std::string& textureName)
{
	auto fill = mHealthBarFill.lock();
	if(fill) fill->SetTexture(textureName);
}

void CHealthSystem::SetHealthBarScale(const Vec2& size)
{
	mHealthBarScale = size;
	if (auto background = mHealthBarBackground.lock()) {
		background->SetScale(size);
	}
	if (auto edge = mHealthBarEdge.lock()) {
		edge->SetScale(size);
	}
	if (auto fill = mHealthBarFill.lock()) {
		fill->SetScale(size);
	}
	UpdateHealthBar();
}
