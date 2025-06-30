#include "stdafx.h"
#include "ParticleEmitter.h"
#include"Timer.h"
#include"Transform.h"
#include"ParticleManager.h"

CParticleEmitter::CParticleEmitter(const ParticleProperties& particleProperties)
{
	mParticleProperties = particleProperties;
	mTimeSinceLastEmit = 0.f;
}

CParticleEmitter::~CParticleEmitter()
{
	INSTANCE(CParticleManager).RemoveParticleEmitter(this);
	mSpawnData.clear();
	mParticles.clear();
	mParticleVertices.clear();
	mParticleProperties.EmitProperties.lastEmitPosW = Vec3::Zero;
	mParticleProperties.EmitProperties.emitPosW = Vec3::Zero;
}

void CParticleEmitter::Initialize()
{
	using namespace RandomNumberGenerator;
	mSpawnData.clear();
	mSpawnData.reserve(mParticleProperties.EmitProperties.maxParticles);
	for (uint32_t i = 0; i < mParticleProperties.EmitProperties.maxParticles; ++i) {
		ParticleSpawnData data;
		data.ageRate = 1.f / RandFloat(mParticleProperties.lifeMinMax.x, mParticleProperties.lifeMinMax.y);
		data.rotationSpeed = RandFloat();
		data.startSize = RandFloat(mParticleProperties.size.x, mParticleProperties.size.y);
		data.endSize = RandFloat(mParticleProperties.size.z, mParticleProperties.size.w);
		float horizontalAngle = RandFloat(XM_2PI);
		float horizontalVelocity = RandFloat(mParticleProperties.velocity.x, mParticleProperties.velocity.y);
		data.velocity.x = horizontalVelocity * cosf(horizontalAngle);
		data.velocity.y = RandFloat(mParticleProperties.velocity.z, mParticleProperties.velocity.w);
		data.velocity.z = horizontalVelocity * sinf(horizontalAngle);
		data.mass = RandFloat(mParticleProperties.massMinMax.x, mParticleProperties.massMinMax.y);
		data.spreadOffset = RandVec3(-mParticleProperties.spread, mParticleProperties.spread);
		data.startColor = RandColor(mParticleProperties.MinStartColor, mParticleProperties.MaxStartColor);
		data.endColor = RandColor(mParticleProperties.MinEndColor, mParticleProperties.MaxEndColor);
		data.random = RandFloat();
		mSpawnData.push_back(data);
	}
	mParticles.clear();
	mParticles.reserve(mParticleProperties.EmitProperties.maxParticles);
	for (uint32_t i = 0; i < mParticleProperties.EmitProperties.maxParticles; ++i) {
		mParticles.emplace_back();
	}
}

void CParticleEmitter::Awake()
{
	
	
}

void CParticleEmitter::Start()
{
	if (mParticleProperties.EmitProperties.maxParticles <= 0) {
		return;
	}
	SetEmitterLocation(GetTransform()->GetWorldPosition());

	INSTANCE(CParticleManager).AddParticleEmitter(this);
}

void CParticleEmitter::Update()
{
	SetEmitterLocation(GetTransform()->GetWorldPosition());
}

int CParticleEmitter::UpdateParticles(ParticleVertex* dataPtr)
{
	float deltaTime = DELTA_TIME;
	mTimeSinceLastEmit += deltaTime;
	if (mTimeSinceLastEmit >= mParticleProperties.emitRate) {
		EmitParticles();
		mTimeSinceLastEmit = 0.f;
	}

	int activeParticleCount = 0;
	for (int i = 0; auto & particle : mParticles) {
		ParticleSpawnData& spawnDataItem = mSpawnData[particle.ResetDataIndex];
		particle.Age += deltaTime * spawnDataItem.ageRate;
		if (particle.Age >= 1.f) {
			continue; 
		}
		particle.Position += particle.Velocity * deltaTime;
		particle.Velocity += mParticleProperties.EmitProperties.gravity * deltaTime * particle.Mass;
		particle.Rotation += spawnDataItem.rotationSpeed * deltaTime;
		dataPtr[activeParticleCount].position = particle.Position;
		dataPtr[activeParticleCount].size = lerp(spawnDataItem.startSize, spawnDataItem.endSize, particle.Age);
		dataPtr[activeParticleCount].color = Color::Lerp(spawnDataItem.startColor, spawnDataItem.endColor, particle.Age);
		dataPtr[activeParticleCount++].color *= particle.Age * (1.0 - particle.Age) * (1.0 - particle.Age) * 6.7;
	}
	return activeParticleCount;
}

void CParticleEmitter::EmitParticles()
{
	for (auto& particle : mParticles) {
		ParticleSpawnData& spawnDataItem = mSpawnData[particle.ResetDataIndex];
		if (particle.Age >= 1.f) {
			Vec3 randDir = spawnDataItem.velocity.x * mParticleProperties.EmitProperties.emitRightW +
				spawnDataItem.velocity.y * mParticleProperties.EmitProperties.emitUpW + spawnDataItem.velocity.z * mParticleProperties.EmitProperties.emitDirW;
			Vec3 emitterVelocity = mParticleProperties.EmitProperties.emitPosW - mParticleProperties.EmitProperties.lastEmitPosW;
			Vec3 newVelocity = randDir + emitterVelocity * mParticleProperties.EmitProperties.emitterVelocitySensitivity;
			particle.Velocity = newVelocity + mParticleProperties.EmitProperties.emitPosW * mParticleProperties.EmitProperties.emitSpeed;
			particle.Position = mParticleProperties.EmitProperties.emitPosW - emitterVelocity * spawnDataItem.random + spawnDataItem.spreadOffset;
			particle.Mass = spawnDataItem.mass;
			particle.Age = 0.f;
			particle.Rotation = 0.f;
			particle.ResetDataIndex = RandomNumberGenerator::RandInt(0, mSpawnData.size() - 1);
		}
	}
}

void CParticleEmitter::Play()
{
}

void CParticleEmitter::Pause()
{
}

void CParticleEmitter::Reset()
{
}
