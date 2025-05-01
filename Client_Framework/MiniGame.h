#pragma once
#include "Scene.h"
class CMiniGame :
    public CScene
{
private:

public:
	CMiniGame();
	~CMiniGame() override = default;
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void RenderScene() override;

};

