#pragma once
#include"stdafx.h"
#include"ParticleEmitter.h"

class CCamera;

class CParticleManager
{
	MAKE_SINGLETON(CParticleManager)

private:
	std::unique_ptr<CStructedBuffer> mParticleVertexBuffer;
	std::vector<CParticleEmitter*> mParticleEmitters;
	int mParticleCount = 0;

	std::shared_ptr<CCamera> mMainCamera;

public:
	void Initialize();
	void Update();
	void Render();

	void AddParticleEmitter(CParticleEmitter* emitter)
	{
		if (emitter) {
			mParticleEmitters.push_back(emitter);
			emitter->Initialize();
		}
	}

	void SetMainCamera(std::shared_ptr<CCamera> camera) { mMainCamera = camera; }
};

