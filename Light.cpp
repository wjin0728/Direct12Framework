#include "stdafx.h"
#include "Light.h"
#include "GameObject.h"
#include "Transform.h"
#include "DX12Manager.h"
#include "Scene.h"
#include"ObjectPoolManager.h"

CLight::~CLight()
{
	if (mLightIndex < 0) {
		return;
	}
	INSTANCE(CObjectPoolManager).ReturnLightCBVIndex(mLightIndex);
	mLightIndex = -1;
	mCbvOffset = 0;
	mLightData = {};
}

std::shared_ptr<CComponent> CLight::Clone()
{
	std::shared_ptr<CLight> copy = std::make_shared<CLight>();
	copy->mLightData = mLightData;
	return copy;
}

void CLight::Awake()
{

}

void CLight::Start()
{
	mLightData.worldMat = owner->GetTransform()->GetWorldMat();
	if (mLightIndex < 0) {
		mLightIndex = INSTANCE(CObjectPoolManager).GetLightCBVIndex();
		mCbvOffset = ALIGNED_SIZE(sizeof(mLightData)) * mLightIndex;
	}


}

void CLight::Update()
{
}

void CLight::LateUpdate()
{
}

void CLight::Render(std::shared_ptr<class CCamera> camera)
{
	if (mLightIndex < 0) return;
	if (!camera) return;

	if(!owner->GetStatic()) mLightData.worldMat = owner->GetTransform()->GetWorldMat(false);
	if (mLightData.type == (UINT)LIGHT_TYPE::DIRECTIONAL) {
		auto& transform = owner->GetTransform();
		Vec3 direction = transform->GetLocalLook();
	}
	else if (mLightData.type == (UINT)LIGHT_TYPE::POINT) {
		auto& transform = owner->GetTransform();
		Vec3 position = transform->GetLocalPosition();
	}
	else if (mLightData.type == (UINT)LIGHT_TYPE::SPOT) {
		auto& transform = owner->GetTransform();
		Vec3 position = transform->GetLocalPosition();
		Vec3 direction = transform->GetLocalLook();
	}
}
