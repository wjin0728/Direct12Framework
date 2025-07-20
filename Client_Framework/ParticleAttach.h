#pragma once
#include "Renderer.h"
#include"ParticleEmitter.h"
#include"UploadBuffer.h"

class CParticleAttach : public CComponent
{
private:
	std::string mParticleEmitterName;
	CParticleEmitter* mParticleEmitter = nullptr;
	float mElapsedTime = 0.0f;

	bool mReserve = true;
	bool mLoop = false;

public:
	bool mCanEmit = true;
	CParticleAttach() = default;
	CParticleAttach(const CParticleAttach& other);
	~CParticleAttach() override;

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CParticleAttach>(*this); };

public:
	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void InitializeParticleEmitter();
	void Play();
	void Stop();
	void Reserve(bool reserve);
	void SetLoop(bool loop);
	void SetParticleEmitterName(const std::string& name) { mParticleEmitterName = name; }
	void SetParticleEmitter(CParticleEmitter* emitter) { mParticleEmitter = emitter; }
};

