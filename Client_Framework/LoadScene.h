#pragma once
#include"stdafx.h"
#include "Scene.h"

class CLoadScene : public CScene
{
public:
	CLoadScene();
	~CLoadScene() = default;
	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void RenderScene() override;
};

