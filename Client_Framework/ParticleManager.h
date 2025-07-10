#pragma once
#include"stdafx.h"
#include"ParticleEmitter.h"

class CCamera;

class CParticleManager
{
	MAKE_SINGLETON(CParticleManager)

private:
	std::unordered_map<std::string, std::unique_ptr<ParticleProperties>> mParticlePropertiesMap;
	std::unique_ptr<CStructedBuffer> mParticleVertexBuffer;
	std::vector<CParticleEmitter*> mActiveParticleEmitters;
	std::vector<std::unique_ptr<CParticleEmitter>> mParticleEmitterPool;
	int mParticleCount = 0;

	std::shared_ptr<CCamera> mMainCamera;
	std::shared_ptr<class CShader> mParticleShader;

public:
	void Initialize(UINT poolSize);
	void LoadParticleProperties();
	void Update();
	void Render();

	void LoadParticleProperties(const std::string& name, const std::string& filePath);
	void AddParticleProperties(const std::string& name, const ParticleProperties& properties);

	CParticleEmitter* GetAvailableParticleEmitter();
	void ReleaseParticleEmitter(CParticleEmitter* emitter);
	void ReleaseAllParticleEmitters();

	void PlayParticleEmitter(const std::string& name, const Vec3& position, bool looping = false);

	void SetMainCamera(std::shared_ptr<CCamera> camera) { mMainCamera = camera; }
};

