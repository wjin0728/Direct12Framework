#include "stdafx.h"
#include "LightManager.h"
#include"DX12Manager.h"
#include"FrameResource.h"
#include"UploadBuffer.h"

void CLightManager::Initialize()
{
}

void CLightManager::AddDirectionalLight(const std::shared_ptr<CDirectionalLight>& dirLight)
{
	directionalLights.push_back(dirLight);
}

void CLightManager::AddPointLight(const std::shared_ptr<CPointLight>& pointLight)
{
	pointLights.push_back(pointLight);
}

void CLightManager::AddSpotLight(const std::shared_ptr<CSpotLight>& spotLight)
{
	spotLights.push_back(spotLight);
}

void CLightManager::Update()
{
	CBLightsData data{};

	for (int i = 0; const auto & dirLight : directionalLights) {
		dirLight->UpdateLightData(data.dirLights[i++]);
	}
	for (int i = 0; const auto & pointLight : pointLights) {
		pointLight->UpdateLightData(data.pointLights[i++]);
	}
	for (int i = 0; const auto & spotLight : spotLights) {
		spotLight->UpdateLightData(data.spotLights[i++]);
	}
	data.lightNum = { (UINT)directionalLights.size(), (UINT)pointLights.size(), (UINT)spotLights.size() };

	auto constantBuffer = UPLOADBUFFER(CONSTANT_BUFFER_TYPE::LIGHT);
	constantBuffer->CopyData(&data);
}

