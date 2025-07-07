#include "stdafx.h"
#include "ParticleManager.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Shader.h"

void CParticleManager::Initialize(UINT poolSize)
{
	UINT particleCount = 100; 
	mParticleVertexBuffer = std::make_unique<CStructedBuffer>();
	mParticleVertexBuffer->Initialize(7, sizeof(ParticleVertex), poolSize * particleCount, nullptr);

	mParticleEmitterPool.resize(poolSize);
	for (size_t i = 0; i < poolSize; ++i) {
		if (mParticleEmitterPool[i]) {
			mParticleEmitterPool[i]->Release(); 
		} 
		else mParticleEmitterPool[i] = std::make_unique<CParticleEmitter>();
	}
}

void CParticleManager::LoadParticleProperties()
{
	LoadParticleProperties("Smoke", OBJECT_PATH("FX_Smoke"));
}

void CParticleManager::Update()
{
	mParticleCount = 0;
	ParticleVertex* particleVerticesPtr = reinterpret_cast<ParticleVertex*>(mParticleVertexBuffer->mappedData);
	for (auto& emitter : mActiveParticleEmitters) {
		if (emitter->IsEnded()) {
			ReleaseParticleEmitter(emitter);
			continue;
		}
		int particleCnt = emitter->UpdateParticles(particleVerticesPtr);
		if (particleCnt > 0) {
			mParticleCount += particleCnt;
			particleVerticesPtr += particleCnt;
		}
	}
	mParticleShader = INSTANCE(CResourceManager).Get<CShader>("ParticleForward");
}

void CParticleManager::Render()
{
	if (!mMainCamera) return;
	if (mParticleCount == 0) return;
	if (!mParticleShader) return;
	mParticleVertexBuffer->BindToShader();
	mParticleShader->SetPipelineState(CMDLIST);

	CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMDLIST->DrawInstanced(4, mParticleCount, 0, 0);
}

void CParticleManager::LoadParticleProperties(const std::string& name, const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open particle properties file: " + filePath);
	}
	using namespace BinaryReader;

	mParticlePropertiesMap[name] = std::make_unique<ParticleProperties>();
	ParticleProperties::ReadParticlePropertiesFromFile(file, *mParticlePropertiesMap[name].get());
}

void CParticleManager::AddParticleProperties(const std::string& name, const ParticleProperties& properties)
{
	if (mParticlePropertiesMap.find(name) != mParticlePropertiesMap.end()) {
		throw std::runtime_error("Particle properties with name '" + name + "' already exists.");
	}
	mParticlePropertiesMap[name] = std::make_unique<ParticleProperties>(properties);
}

CParticleEmitter* CParticleManager::GetAvailableParticleEmitter()
{
	for (auto& emitter : mParticleEmitterPool) {
		if (!emitter->mIsActive) {
			emitter->Release();
			emitter->mIsActive = true;
			mActiveParticleEmitters.push_back(emitter.get());
			return emitter.get();
		}
	}
	return nullptr;
}

void CParticleManager::ReleaseParticleEmitter(CParticleEmitter* emitter)
{
	if (!emitter) return;
	auto it = std::find(mActiveParticleEmitters.begin(), mActiveParticleEmitters.end(), emitter);
	if (it != mActiveParticleEmitters.end()) {
		mActiveParticleEmitters.erase(it);
		emitter->mIsActive = false;
	}
}

void CParticleManager::PlayParticleEmitter(const std::string& name, const Vec3& position, bool looping)
{
	CParticleEmitter* emitter = GetAvailableParticleEmitter();
	if (emitter) {
		emitter->mIsActive = true;
		emitter->mIsLooping = looping;
		emitter->Play(position);
	}
}

