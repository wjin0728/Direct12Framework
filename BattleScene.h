#pragma once
#include"stdafx.h"
#include"Scene.h"
#include"LightManager.h"
#include"FrameResource.h"

class CBattleScene : public CScene
{
private:
	std::unique_ptr<CLightManager> lightMgr{};
	std::vector<std::shared_ptr<class CInstancingGroup>> instancingGroups{};

public:
	CBattleScene();
	virtual ~CBattleScene() = default;

	virtual void Initialize();

	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();

private:
	void SetLights();
};

