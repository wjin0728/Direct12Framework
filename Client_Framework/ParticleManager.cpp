#include "stdafx.h"
#include "ParticleManager.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Shader.h"
#include"Camera.h"

void CParticleManager::Initialize(UINT poolSize)
{
	UINT particleCount = 500; 
	mParticleVertexBuffer = std::make_unique<CStructedBuffer>();
	mParticleVertexBuffer->Initialize(7, sizeof(ParticleVertex), poolSize * particleCount, nullptr);

	mParticleEmitterPool.resize(poolSize);
	for (size_t i = 0; i < poolSize; ++i) {
		if (mParticleEmitterPool[i]) {
			mParticleEmitterPool[i]->Release(); 
		} 
		else mParticleEmitterPool[i] = std::make_unique<CParticleEmitter>(particleCount);
	}
	
}

void CParticleManager::LoadParticleProperties()
{
	//LoadParticleProperties("DeathSmoke", OBJECT_PATH("FX_Smoke"));
	LoadParticleProperties("FootDust", OBJECT_PATH("FootDust"));
	//LoadParticleProperties("MonsterAttack", OBJECT_PATH("MonsterAttack"));
	//LoadParticleProperties("MonsterDeath", OBJECT_PATH("MonsterDeath"));
	LoadParticleProperties("ArrowHit", OBJECT_PATH("ArrowHit"));
}

void CParticleManager::Update()
{
	mParticleShader = INSTANCE(CResourceManager).Get<CShader>("ParticleForward");
	mParticleCount = 0;
	ParticleVertex* particleVerticesPtr = reinterpret_cast<ParticleVertex*>(mParticleVertexBuffer->mappedData);
	for (auto& emitter : mActiveParticleEmitters) {
		if (emitter->IsEnded()) {
			ReleaseParticleEmitter(emitter);
			continue;
		}
		int particleCnt = emitter->UpdateParticles(particleVerticesPtr, mMainCamera);
		if (particleCnt > 0) {
			mParticleCount += particleCnt;
			particleVerticesPtr += particleCnt;
		}
	}
	if (mParticleCount > 0) {
		if (!mMainCamera) return;
		ParticleVertex* particleVertices = reinterpret_cast<ParticleVertex*>(mParticleVertexBuffer->mappedData);
		std::sort(particleVertices, particleVertices + mParticleCount, [&](const ParticleVertex& a, const ParticleVertex& b) {
			return a.distanceToCamera > b.distanceToCamera; 
			});
	} 
}

void CParticleManager::Render()
{
	//if (!mMainCamera) return;
	if (mParticleCount == 0) return;
	if (!mParticleShader) return;
	mParticleVertexBuffer->BindToShader();
	mParticleShader->SetPipelineState(CMDLIST);

	if (mParticleCount > 500)
		int i = 0;
	CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
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
		return;
	}
	mParticlePropertiesMap[name] = std::make_unique<ParticleProperties>(properties);
}

CParticleEmitter* CParticleManager::GetAvailableParticleEmitter()
{
	for (auto& emitter : mParticleEmitterPool) {
		if (!emitter->mIsPlaying && (emitter->mActiveParticleCount == 0)) {
			return emitter.get();
		}
	}
	return nullptr;
}

CParticleEmitter* CParticleManager::GetAvailableParticleEmitter(const std::string& name)
{
	auto it = mParticlePropertiesMap.find(name);
	if (it == mParticlePropertiesMap.end()) {
		throw std::runtime_error("Particle properties with name '" + name + "' not found.");
	}
	CParticleEmitter* emitter = GetAvailableParticleEmitter();
	if (emitter) {
		emitter->Initialize(it->second.get());
		return emitter;
	}
	return nullptr;
}

void CParticleManager::ReleaseParticleEmitter(CParticleEmitter* emitter)
{
	if (!emitter) return;
	auto it = std::find(mActiveParticleEmitters.begin(), mActiveParticleEmitters.end(), emitter);
	if (it != mActiveParticleEmitters.end()) {
		mActiveParticleEmitters.erase(it);
		emitter->Release();
	}
}

void CParticleManager::ReleaseAllParticleEmitters()
{
	for (auto& emitter : mParticleEmitterPool) {
		emitter->Release();
	}
	mActiveParticleEmitters.clear();
	mParticleCount = 0;
}

void CParticleManager::PlayParticleEmitter(CParticleEmitter* emitter)
{
	if (emitter) {
		if(!emitter->mIsPlaying) mActiveParticleEmitters.push_back(emitter);
		emitter->Play();
	}
}

CParticleEmitter* CParticleManager::PlayParticleEmitter(const std::string& name, const Vec3& position, bool looping)
{
	CParticleEmitter* emitter = GetAvailableParticleEmitter();
	if (emitter) {
		auto it = mParticlePropertiesMap.find(name);
		if (it == mParticlePropertiesMap.end()) {
			throw std::runtime_error("Particle properties with name '" + name + "' not found.");
		}
		mActiveParticleEmitters.push_back(emitter);
		emitter->Initialize(it->second.get());
		emitter->mIsLooping = looping;
		emitter->Play(position);
		return emitter;
	}
	return nullptr;
}

CParticleEmitter* CParticleManager::PlayParticleEmitter(const std::string& name, const Matrix& mat, bool looping)
{
	CParticleEmitter* emitter = GetAvailableParticleEmitter(name);
	if (emitter) {
		auto it = mParticlePropertiesMap.find(name);
		if (it == mParticlePropertiesMap.end()) {
			throw std::runtime_error("Particle properties with name '" + name + "' not found.");
		}
		mActiveParticleEmitters.push_back(emitter);
		emitter->mIsLooping = looping;
		emitter->Play(mat);
		return emitter;
	}
	return nullptr;
}

