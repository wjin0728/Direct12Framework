#pragma once
#include "Scene.h"

class CMainScene : public CScene
{
public:
	CMainScene();
	virtual ~CMainScene() = default;

	virtual void Initialize() override;

	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void RenderScene() override;
};
