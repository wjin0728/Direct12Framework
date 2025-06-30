#include "stdafx.h"
#include "ParticleManager.h"
#include"DX12Manager.h"

void CParticleManager::Initialize()
{
	mParticleVertexBuffer = std::make_unique<CStructedBuffer>();
	mParticleVertexBuffer->Initialize(7, sizeof(ParticleVertex), 10000, nullptr);										
}

void CParticleManager::Update()
{
	mParticleCount = 0;
	ParticleVertex* particleVerticesPtr = reinterpret_cast<ParticleVertex*>(mParticleVertexBuffer->mappedData);
	for (auto& emitter : mParticleEmitters) {
		int particleCnt = emitter->UpdateParticles(particleVerticesPtr);
		if (particleCnt > 0) {
			mParticleCount += particleCnt;
			particleVerticesPtr += particleCnt;
		}
	}
}

void CParticleManager::Render()
{
	if (!mMainCamera) return;
	mParticleVertexBuffer->BindToShader();

	CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMDLIST->DrawInstanced(4, mParticleCount, 0, 0);
}

