#pragma once
#include "MonoBehaviour.h"
class CScrollTexture : public CMonoBehaviour
{
private:
	Vec3 movement = Vec3::Zero;

public:
	CScrollTexture();
	~CScrollTexture();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CScrollTexture>(); }
};

