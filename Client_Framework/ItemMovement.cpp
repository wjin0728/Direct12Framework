#include "stdafx.h"
#include "ItemMovement.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Camera.h"
#include"ResourceManager.h"
#include"MeshRenderer.h"
#include "Material.h"

void CItemMovement::Awake()
{
}

void CItemMovement::Start()
{
	auto transform = GetTransform();
	if (mDirection == Vec3(0.f, 0.f, 0.f)) {
		mDirection = transform->GetLocalUp();
	}
	if (mStartPos == Vec3(0.f, 0.f, 0.f)) {
		mStartPos = transform->GetLocalPosition();
	}

	std::string itemName{};
	Color itemColor = Color(1.f, 1.f, 1.f, 1.f);
	switch (mItemType)
	{
	case FIRE_ENCHANT:
		itemName = "FireEnchant";
		itemColor = Color(0.7f, 0.2f, 0.f, 1.f);
		break;
	case FIRE_EXPLOSION:
		itemName = "FireExplosion";
		itemColor = Color(1.f, 0.2f, 0.f, 1.f);
		break;
	case WATER_HEAL:
		itemName = "WaterHeal";
		itemColor = Color(0.f, 0.2f, 1.f, 1.f);
		break;
	case WATER_SHIELD:
		itemName = "Water_Shield";
		itemColor = Color(0.f, 0.2f, 1.f, 1.f);
		break;
	case GRASS_VINE:
		itemName = "GrassVine";
		itemColor = Color(0.f, 1.f, 0.f, 1.f);
		break;
	case GRASS_WEAKEN:
		itemName = "GrassWeaken";
		itemColor = Color(0.f, 1.f, 0.f, 1.f);
		break;
	case item_end:
		break;
	default:
		break;
	}

	owner->mCastShadow = false;
	if (auto renderer = owner->GetComponent<CMeshRenderer>()) {
		if (auto mat = renderer->GetMaterial()->Instantiate()) {
			mat->EnrollToPool();
			mat->SetProperty("mainColor", itemColor);
			UINT texIdx = RESOURCE.GetTextureIndex("TranslucentCrystal01_Emission");
			mat->SetProperty("mainTexIdx", texIdx);
			renderer->SetMaterial(mat);
		}
	}

	mOutSide = owner->FindChildByName("Capsule");
	if(mOutSide.lock()) {
		mOutSide.lock()->mCastShadow = false;
		if (auto renderer = mOutSide.lock()->GetComponent<CMeshRenderer>()) {
			if (auto mat = RESOURCE.Get<CMaterial>("ItemDefault")->Instantiate()) {
				auto texture = RESOURCE.Get<CTexture>(itemName);
				mat->SetProperty("mainTexIdx", texture->GetSrvIndex());
				mat->SetProperty("tiling", Vec2(3.f,1.f));
				mat->SetProperty("smoothness", 0.0f);
				mat->SetProperty("metallic", 0.0f);
				//itemColor.w = 0.5f; // Set alpha to 0.5 for the capsule
				mat->SetProperty("mainColor", itemColor);
				renderer->SetMaterial(mat);
			}
		}
	}
}

void CItemMovement::Update()
{
	auto transform = GetTransform();
	Vec3 pos = mStartPos;
	pos += mDirection * mAmplitude * sinf(TIMER.GetTotalTime() * mFrequency);
	transform->SetLocalPosition(pos);

	if(auto outSide = mOutSide.lock()) {
		outSide->GetTransform()->RotateY(180 * TIMER.GetDeltaTime());
	}
}

void CItemMovement::LateUpdate()
{
}
