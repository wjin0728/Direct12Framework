#pragma once
#include"stdafx.h"
#include "Scene.h"

class CMainScene : public CScene
{
public:
	CMainScene();
	virtual ~CMainScene() = default;

	virtual void Initialize();

	virtual void Update();
	virtual void Render();
};

