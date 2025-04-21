#include "stdafx.h"
#include "Light.h"
#include "GameObject.h"
#include "Transform.h"
#include "DX12Manager.h"
#include "Scene.h"

std::shared_ptr<CComponent> CLight::Clone()
{
	std::shared_ptr<CLight> copy = std::make_shared<CLight>();
	copy->lightData = lightData;
	return copy;
}

void CLight::Awake()
{

}

void CLight::Start()
{
}

void CLight::Update()
{
}

void CLight::LateUpdate()
{
}
