#pragma once
#include "Renderer.h"
#include"ParticleEmitter.h"
#include"UploadBuffer.h"

class CParticleAttach : public CComponent
{
private:
	std::shared_ptr<CParticleEmitter> mParticleEmitter = nullptr;
	float mElapsedTime = 0.0f;

public:
	CParticleAttach() = default;
	~CParticleAttach() override = default;

public:
	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;
};

