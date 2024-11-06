#pragma once
#include "Scene.h"

class CMenuScene : public CScene
{
public:
	CMenuScene();
	virtual ~CMenuScene() = default;

	virtual void Initialize();

	virtual void Update();
	virtual void Render();
};

