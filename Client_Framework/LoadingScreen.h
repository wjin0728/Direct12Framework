#pragma once
#include "stdafx.h"
#include "Component.h"

class CLoadingScreen : public CComponent
{
private:
	float mTime{ 0.f };
	std::vector<int> mLoadingScreenTextureIdx{};
	int mLoadingScreenIndex{ -1 };
	float mLoadingScreenSpeed{ 0.5f };
	std::shared_ptr<class CUIRenderer> renderer{ nullptr };
	float mFadeOutTime{ 1.f };
	bool mIsLoading{ true };

public:
	CLoadingScreen();
	~CLoadingScreen() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	std::shared_ptr<CComponent> Clone() override { return std::make_shared<CLoadingScreen>(*this); }
public:
	void SetLoadingScreenIndex(int index) { mLoadingScreenIndex = index; }
	void SetLoadingScreenSpeed(float speed) { mLoadingScreenSpeed = speed; }
	void SetIsLoading(bool isLoading) { mIsLoading = isLoading; }
	void AddLoadingScreenTextureIdx(int idx) { mLoadingScreenTextureIdx.push_back(idx); }

	void SetRenderer(std::shared_ptr<class CUIRenderer> _renderer) { renderer = _renderer; }
	int GetLoadingScreenIndex() { return mLoadingScreenIndex; }
	std::shared_ptr<class CUIRenderer> GetRenderer() { return renderer; }
	float GetFadeOutTime() { return mFadeOutTime; }

};

