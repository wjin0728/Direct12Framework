#pragma once
#include"stdafx.h"
#include "Scene.h"

class CLoadScene : public CScene
{
private:
	std::shared_ptr<class CLoadingScreen> mLoadingScreen{ nullptr };
	bool mIsLoading{ false };
public:
	CLoadScene();
	~CLoadScene() = default;
	virtual void Initialize() override;
	virtual void Start() override; 
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void RenderScene() override;

};

