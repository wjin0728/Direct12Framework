#pragma once
#include "Renderer.h"
#include"ParticleEmitter.h"
#include"UploadBuffer.h"

class CParticleRenderer : public CRenderer
{
private:
	std::shared_ptr<CParticleEmitter> mParticleEmitter = nullptr;
	float mElapsedTime = 0.0f;

	std::shared_ptr<CStructedBuffer> mParticleBuffer = nullptr;

	public:
	CParticleRenderer() = default;
	~CParticleRenderer() override = default;

public:
	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual void Render(std::shared_ptr<CCamera> camera, int pass = 0);
	virtual void UpdataObjectDataToShader();
};

