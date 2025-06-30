#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"
#include"Component.h"

struct ParticleVertex
{
	Vec3 position;
	Color color;
	float size;
	int albedoTexIdx;
};

struct ParticleProperties
{
	EmitterProperties  EmitProperties;
	Color MinStartColor;
	Color MaxStartColor;
	Color MinEndColor;
	Color MaxEndColor;
	float emitRate;
	Vec2 lifeMinMax;
	Vec2 massMinMax;
	Vec4 size;
	Vec3 spread;
	Vec4 velocity;
	std::wstring TexturePath;
	float totalActiveLifetime;
};

class CParticleEmitter : public CComponent
{
public:
	ParticleProperties mParticleProperties;
    float mTimeSinceLastEmit = 0.f;

	std::vector<ParticleSpawnData> mSpawnData;
    std::vector<ParticleMotion> mParticles;
	std::vector<ParticleVertex> mParticleVertices;

	bool mIsPlaying = true;

public:
	CParticleEmitter() = default;
	CParticleEmitter(const ParticleProperties& particleProperties);
	~CParticleEmitter();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CParticleEmitter>(*this); }
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void Initialize();
	int UpdateParticles(ParticleVertex* dataPtr);
	void EmitParticles();
	void Play();
	void Pause();
	void Reset();

	void SetEmitterLocation(const Vec3& location) { 
		mParticleProperties.EmitProperties.lastEmitPosW = mParticleProperties.EmitProperties.emitPosW;
		mParticleProperties.EmitProperties.emitPosW = location; }

	int GetParticleCount() const { return mParticleProperties.EmitProperties.maxParticles; }
};

