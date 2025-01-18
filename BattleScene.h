#pragma once
#include"stdafx.h"
#include"Scene.h"
#include"LightManager.h"
#include"FrameResource.h"

class CBattleScene : public CScene
{
private:
	std::shared_ptr<CGameObject> dirLightObj{};
	std::shared_ptr<CDirectionalLight> dirLight{};

public:
	CBattleScene();
	virtual ~CBattleScene() = default;

	virtual void Initialize();

	virtual void Update();
	virtual void LateUpdate();

private:

	void SetLights();
};

