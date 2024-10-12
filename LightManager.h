#pragma once
#include"stdafx.h"
#include"Light.h"


class CLightManager
{
private:
	std::vector<std::shared_ptr<CDirectionalLight>> directionalLights;
	std::vector<std::shared_ptr<CPointLight>> pointLights;
	std::vector<std::shared_ptr<CSpotLight>> spotLights;

public:
	CLightManager() = default;
	~CLightManager() = default;

public:
	void Initialize();

	void AddDirectionalLight(const std::shared_ptr<CDirectionalLight>& dirLight);
	void AddPointLight(const std::shared_ptr<CPointLight>& pointLight);
	void AddSpotLight(const std::shared_ptr<CSpotLight>& spotLight);

	void Update();
};

