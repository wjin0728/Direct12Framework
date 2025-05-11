#pragma once
#include"stdafx.h"
#include "Scene.h"

class CLoadScene : public CScene
{
private:
	std::shared_ptr<class CLoadingScreen> mLoadingScreen{ nullptr };

public:
	CLoadScene();
	~CLoadScene() = default;
	void Initialize() override;
	void Awake() override;
	void Start() override;

	void Update() override;
	void LateUpdate() override;
	void RenderScene() override;

};

