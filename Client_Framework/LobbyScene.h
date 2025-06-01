#pragma once
#include"stdafx.h"
#include "Scene.h"

class CLobbyScene : public CScene
{
public:
	CLobbyScene();
	virtual ~CLobbyScene() = default;

	virtual void Initialize();

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual void RenderScene() override;
};

