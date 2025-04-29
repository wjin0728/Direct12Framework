#pragma once
#include"stdafx.h"
#include"Scene.h"
#include"FrameResource.h"

class CBattleScene : public CScene
{
private:
	static constexpr UINT CASCADE_COUNT = 3;

public:
	CBattleScene();
	virtual ~CBattleScene() = default;

	virtual void Initialize();

	virtual void Update();
	virtual void LateUpdate();

	virtual void RenderScene();

private:
};

