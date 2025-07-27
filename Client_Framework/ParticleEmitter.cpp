#include "stdafx.h"
#include "ParticleEmitter.h"
#include"Timer.h"
#include"Transform.h"
#include"ParticleManager.h"
#include"ResourceManager.h"
#include"Texture.h"
#include"Camera.h"

Vec3 Gradient::EvaluateColor(float t) const
{
	if (colorKeys.empty()) return Vec3(1, 1, 1);

	if (t <= colorKeys.front().time) return colorKeys.front().color;
	if (t >= colorKeys.back().time)  return colorKeys.back().color;

	for (size_t i = 1; i < colorKeys.size(); ++i)
	{
		if (t < colorKeys[i].time)
		{
			float t0 = colorKeys[i - 1].time;
			float t1 = colorKeys[i].time;
			float f = (t - t0) / (t1 - t0);
			Vec3 c0 = colorKeys[i - 1].color;
			Vec3 c1 = colorKeys[i].color;
			return Vec3::Lerp(c0, c1, f);
		}
	}
	return colorKeys.back().color;
}

float Gradient::EvaluateAlpha(float t) const
{
	if (alphaKeys.empty()) return 1.0f;
	if (t <= alphaKeys.front().time) return alphaKeys.front().alpha;
	if (t >= alphaKeys.back().time)  return alphaKeys.back().alpha;
	for (size_t i = 1; i < alphaKeys.size(); ++i)
	{
		if (t < alphaKeys[i].time)
		{
			float t0 = alphaKeys[i - 1].time;
			float t1 = alphaKeys[i].time;
			float f = (t - t0) / (t1 - t0);
			float a0 = alphaKeys[i - 1].alpha;
			float a1 = alphaKeys[i].alpha;
			return lerp(a0, a1, f);
		}
	}
	return alphaKeys.back().alpha;
}


CParticleEmitter::CParticleEmitter(UINT maxParticleNum)
{
	mParticleProperties = nullptr;
	mParticles.reserve(maxParticleNum);
	mSpawnData.reserve(maxParticleNum*1.5);
	mTimeSinceLastEmit = 0.f;
}

CParticleEmitter::~CParticleEmitter()
{
	mSpawnData.clear();
	mParticles.clear();
	mLastEmitPosW = Vec3(0.f, 0.f, 0.f);
	mEmitterTransform = Matrix::Identity;
}

void CParticleEmitter::Initialize(ParticleProperties* particleProperties)
{
	mParticleProperties = particleProperties;
	if (mParticleProperties->maxParticles <= 0) {
		return;
	}
	mBurstRec.clear();
	mBurstRec.resize(mParticleProperties->bursts.size(), BurstRecord(true, 0, 0.f));
	mSpawnData.clear();
	for (uint32_t i = 0; i < mParticleProperties->maxParticles * 2; ++i) {
		ParticleSpawnData data;
		data.ageRate = 1.f / mParticleProperties->startLifetimeCurve.GetRandomValue(RandomNumberGenerator::RandFloat(0.f, 1.f));
		data.rotationSpeed = mParticleProperties->useRotationOverTime ? mParticleProperties->rotationOverTimeCurve->GetRandomValue(RandomNumberGenerator::RandFloat(0.f, 1.f)) : 0.f;
		data.startLocation = mParticleProperties->EmitShapeModule.GetRandomPosition();
		data.direction = mParticleProperties->EmitShapeModule.GetRandomDirection();
		data.startRotation = mParticleProperties->startRotationCurve.GetRandomValue(RandomNumberGenerator::RandFloat(0.f, 1.f));
		data.startSize = mParticleProperties->startSizeCurve.GetRandomValue(RandomNumberGenerator::RandFloat(0.f, 1.f));
		data.speed = mParticleProperties->startSpeedCurve.GetRandomValue(RandomNumberGenerator::RandFloat(0.f, 1.f));
		data.startColor = mParticleProperties->startColorGradient.GetRandomColor(RandomNumberGenerator::RandFloat(0.f, 1.f));
		data.random = RandomNumberGenerator::RandFloat();
		mSpawnData.push_back(data);
	}
	mParticles.clear();
	for (uint32_t i = 0; i < mParticleProperties->maxParticles; ++i) {
		mParticles.emplace_back();
	}
}

void CParticleEmitter::Release()
{
	mTimeSinceLastEmit = 0.f;
	mTotalTime = 0.f;
	mIsPlaying = false;
	mIsPaused = false;
	mLastEmitPosW = Vec3(0.f, 0.f, 0.f);
	mEmitterTransform = Matrix::Identity;
	mParticleProperties = nullptr;
	mSpawnData.clear();
	mParticles.clear();
}

int CParticleEmitter::UpdateParticles(ParticleVertex* dataPtr, CCamera* camera)
{
	if (mParticleProperties->maxParticles <= 0) {
		return 0;
	}
	EmitParticles();

	float deltaTime = DELTA_TIME;
	if(!mIsPaused){
		mTotalTime += deltaTime;
	}

	Vec3 cameraPos = camera ? camera->GetLocalPosition() : Vec3(0.f, 0.f, 0.f);
	Vec3 cameraForward = camera ? camera->GetLook() : Vec3(0.f, 0.f, 1.f);
	mActiveParticleCount = 0;
	for (int i = 0; auto & particle : mParticles) {
		ParticleSpawnData& spawnDataItem = mSpawnData[particle.ResetDataIndex];
		particle.Age += deltaTime * spawnDataItem.ageRate;
		if (particle.Age >= 1.f) {
			continue; 
		}
		particle.Velocity += mParticleProperties->gravity * deltaTime;
		if(mParticleProperties->useVelocityOverTime) {
			float x = mParticleProperties->velocityOverTimeCurveX->GetRandomValue(particle.Age);
			float y = mParticleProperties->velocityOverTimeCurveY->GetRandomValue(particle.Age);
			float z = mParticleProperties->velocityOverTimeCurveZ->GetRandomValue(particle.Age);
			particle.Position += Vec3(x, y, z) * deltaTime;
		}
		particle.Position += particle.Velocity * deltaTime;
		particle.rotation += spawnDataItem.rotationSpeed * deltaTime;
		
		dataPtr[mActiveParticleCount].position = particle.Position;
		dataPtr[mActiveParticleCount].size = mParticleProperties->useSizeOverTime ? mParticleProperties->sizeOverTimeCurve->GetRandomValue(particle.Age) * spawnDataItem.startSize : spawnDataItem.startSize;
		dataPtr[mActiveParticleCount].velocity = particle.Velocity;
		dataPtr[mActiveParticleCount].color = mParticleProperties->useColorOverTime ? mParticleProperties->colorOverTimeGradient->GetRandomColor(particle.Age) * spawnDataItem.startColor : spawnDataItem.startColor;
		dataPtr[mActiveParticleCount].rotation = particle.rotation;
		dataPtr[mActiveParticleCount].albedoTexIdx = mParticleProperties->textureIdx;
		dataPtr[mActiveParticleCount].distanceToCamera = (dataPtr[mActiveParticleCount].position - cameraPos).Dot(cameraForward);
		dataPtr[mActiveParticleCount].alignment = mParticleProperties->alignment;

		if (mParticleProperties->useTextureSheetAnimation) {
			float t = mParticleProperties->texSheetAnimationCurve.GetRandomValue(particle.Age);
			dataPtr[mActiveParticleCount].tileX = mParticleProperties->tileX;
			dataPtr[mActiveParticleCount].tileY = mParticleProperties->tileY;
			dataPtr[mActiveParticleCount].frameIdx = std::floor(t * (mParticleProperties->tileX * mParticleProperties->tileX));
		}
		else {
			dataPtr[mActiveParticleCount].tileX = 1;
			dataPtr[mActiveParticleCount].tileY = 1;
			dataPtr[mActiveParticleCount].frameIdx = 0;
		}
		mActiveParticleCount++;
	}
	return mActiveParticleCount;
}

void CParticleEmitter::EmitParticles()
{
	if(!mIsLooping && (mTotalTime >= mParticleProperties->duration)) return; 
	if(mIsPaused) return; 

	float deltaTime = DELTA_TIME;

	for (int i = 0; i < mParticleProperties->bursts.size();i++) {
		BurstRecord& burstRec = mBurstRec[i];
		if (!burstRec.isActive) continue; // Skip if burst is not active
		const Burst& burst = mParticleProperties->bursts[i];
		burstRec.rate += burst.interval;
		if (mTotalTime >= burst.time && burst.interval <= burstRec.rate) {
			burstRec.rate = 0.f; // Reset rate for next burst
			burstRec.count++;
			if (burstRec.count >= burst.cycleTime) {
				burstRec.isActive = false; 
			}
			int burstCount = burst.count.GetRandomValue(0.5);
			for (int j = 0; j < burstCount; ++j) {
				CreateParticle();
			}
		}
	}

	float emitRate = 0.f;
	if (mParticleProperties->emitRate.type == MinMaxCurve::CurveType::Constant) {
		float rate = std::get<float>(mParticleProperties->emitRate.data);
		if (rate <= 0.f) return; // No particles to emit
		emitRate = 1.f / rate;
	}
	else if (mParticleProperties->emitRate.type == MinMaxCurve::CurveType::Curve) {
		float t_curve = mParticleProperties->duration / mTotalTime;
		if (t_curve <= 0.f) return; // No particles to emit
		float rate = mParticleProperties->emitRate.GetRandomValue(t_curve);
		if (rate <= 0.f) return; // No particles to emit
		emitRate = 1.f / mParticleProperties->emitRate.GetRandomValue(t_curve);
	}


	mTimeSinceLastEmit += deltaTime;
	if (!mIsPaused && (mTimeSinceLastEmit >= emitRate)) {
		mTimeSinceLastEmit -= emitRate;
		CreateParticle();
	}
}

void CParticleEmitter::CreateParticle()
{
	for (auto& particle : mParticles) {
		if (particle.Age >= 1.f) {
			particle.ResetDataIndex = RandomNumberGenerator::RandInt(0, mSpawnData.size() - 1);
			ParticleSpawnData& spawnDataItem = mSpawnData[particle.ResetDataIndex];
			Vec3 direction = Vec3::TransformNormal(spawnDataItem.direction, mEmitterTransform);
			particle.Velocity = direction * spawnDataItem.speed;
			particle.rotation = spawnDataItem.startRotation;
			particle.Position = spawnDataItem.startLocation + Vec3(mEmitterTransform._41, mEmitterTransform._42, mEmitterTransform._43);
			particle.Age = 0.f;
			break;
		}
	}
}

void CParticleEmitter::Play(const Vec3& pos)
{
	SetEmitterLocation(pos);
	Play();
}

void CParticleEmitter::Play()
{
	if (mParticleProperties == nullptr || mParticleProperties->maxParticles <= 0) {
		return;
	}
	mIsPlaying = true;
	mIsPaused = false;
	mTimeSinceLastEmit = 0.f;
	mTotalTime = 0.f;
	for (auto& particle : mParticles) {
		particle.ResetDataIndex = RandomNumberGenerator::RandInt(0, mSpawnData.size() - 1);
		particle.Age = 1.f;
	}
	mBurstRec.clear();
	mBurstRec.resize(mParticleProperties->bursts.size(), BurstRecord(true, 0, 0.f));
}

void CParticleEmitter::Pause()
{
	mIsPaused = true;
	mIsPlaying = false;
}

void CParticleEmitter::Resume()
{
	mIsPaused = false;
	mIsPlaying = true;
	mTimeSinceLastEmit = 0.f;
	for (auto& particle : mParticles) {
		particle.ResetDataIndex = RandomNumberGenerator::RandInt(0, mSpawnData.size() - 1);
	}
	mLastEmitPosW = Vec3(mEmitterTransform._41, mEmitterTransform._42, mEmitterTransform._43);
}

void CParticleEmitter::Reset()
{
	mIsPlaying = false;
	mIsPaused = false;
	mTimeSinceLastEmit = 0.f;
	mTotalTime = 0.f;
	for (auto& particle : mParticles) {
		particle.ResetDataIndex = RandomNumberGenerator::RandInt(0, mSpawnData.size() - 1);
		particle.Age = 1.f;
	}
	mBurstRec.clear();
	mBurstRec.resize(mParticleProperties->bursts.size(), BurstRecord(true, 0, 0.f));
	mLastEmitPosW = Vec3(0.f, 0.f, 0.f);
	mEmitterTransform = Matrix::Identity;
}

void CParticleEmitter::Stop(bool reset)
{
	if(reset) {
		Reset();
	}
	else {
		mIsPaused = false;
		mIsLooping = false;
		mTimeSinceLastEmit = 0.f;
		mTotalTime = mParticleProperties ? mParticleProperties->duration : 0.f;
	}
}

void ParticleProperties::ReadParticlePropertiesFromFile(std::ifstream& ifs, ParticleProperties& properties)
{
	using namespace BinaryReader;

	std::string token;
	while (true) {
		ReadDateFromFile(ifs, token);
		if (token == "<Alignment>:") {
			ReadDateFromFile(ifs, properties.alignment);
		}
		if (token == "<Burst>:") {
			int burstCount = 0;
			ReadDateFromFile(ifs, burstCount);
			properties.bursts.resize(burstCount);
			for (int i = 0; i < burstCount; ++i) {
				Burst::ReadBurstFromFile(ifs, properties.bursts[i]);
			}
		}
		else if (token == "<Duration>:") {
			ReadDateFromFile(ifs, properties.duration);
		}
		else if (token == "<MaxParticles>:") {
			ReadDateFromFile(ifs, properties.maxParticles);
		}
		else if (token == "<RateOverTime>:") {
			MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.emitRate);
		}
		else if (token == "<StartColor>:") {
			MinMaxGradient::ReadMinMaxGradientFromFile(ifs, properties.startColorGradient);
		}
		else if (token == "<StartSize>:") {
			MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.startSizeCurve);
		}
		else if (token == "<StartSpeed>:") {
			MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.startSpeedCurve);
		}
		else if (token == "<StartRotation>:") {
			MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.startRotationCurve);
		}
		else if (token == "<StartLifetime>:") {
			MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.startLifetimeCurve);
		}
		else if (token == "<UseColorOvetLifeTime>:") {
			ReadDateFromFile(ifs, properties.useColorOverTime);
			if (properties.useColorOverTime) {
				properties.colorOverTimeGradient = std::make_shared<MinMaxGradient>();
				MinMaxGradient::ReadMinMaxGradientFromFile(ifs, *properties.colorOverTimeGradient);
			}
			else properties.colorOverTimeGradient = nullptr;
		}
		else if (token == "<UseSizeOvetLifeTime>:") {
			ReadDateFromFile(ifs, properties.useSizeOverTime);
			if (properties.useSizeOverTime) {
				properties.sizeOverTimeCurve = std::make_shared<MinMaxCurve>();
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, *properties.sizeOverTimeCurve);
			}
			else properties.sizeOverTimeCurve = nullptr;
		}
		else if (token == "<UseRotationOvetLifeTime>:") {
			ReadDateFromFile(ifs, properties.useRotationOverTime);
			if (properties.useRotationOverTime) {
				properties.rotationOverTimeCurve = std::make_shared<MinMaxCurve>();
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, *properties.rotationOverTimeCurve);
			}
			else properties.rotationOverTimeCurve = nullptr;
		}
		else if (token == "<UseVelocityOvetLifeTime>:") {
			ReadDateFromFile(ifs, properties.useVelocityOverTime);
			if (properties.useVelocityOverTime) {
				properties.velocityOverTimeCurveX = std::make_shared<MinMaxCurve>();
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, *properties.velocityOverTimeCurveX);
				properties.velocityOverTimeCurveY = std::make_shared<MinMaxCurve>();
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, *properties.velocityOverTimeCurveY);
				properties.velocityOverTimeCurveZ = std::make_shared<MinMaxCurve>();
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, *properties.velocityOverTimeCurveZ);
			}
			else {
				properties.velocityOverTimeCurveX = nullptr;
				properties.velocityOverTimeCurveY = nullptr;
				properties.velocityOverTimeCurveZ = nullptr;
			}

		}
		else if (token == "<AlbedoTex>:") {
			std::string texName;
			ReadDateFromFile(ifs, texName);
			if (texName == "null") {
				properties.textureIdx = -1;
				continue;
			}
			if (RESOURCE.Get<CTexture>(texName)) {
				properties.textureIdx = RESOURCE.Get<CTexture>(texName)->GetSrvIndex();
				continue;
			}
			std::string path = TEXTURE_PATH(texName);
			auto mainTex = std::make_shared<CTexture>(texName, path);
			RESOURCE.Add(mainTex);

			properties.textureIdx = mainTex->GetSrvIndex();
		}
		else if (token == "<Gravity>:") {
			ReadDateFromFile(ifs, properties.gravity);
		}
		else if(token == "<UseShape>:") {
			bool useShape = false;
			ReadDateFromFile(ifs, useShape);
			if( useShape ) {
				ReadDateFromFile(ifs, token);
				ShapeModule::ReadShapeModuleFromFile(ifs, properties.EmitShapeModule);
			}
			else {
				properties.EmitShapeModule.type = ShapeModule::ShapeType::Point;
			}
			
		}
		else if (token == "<UseTextureSheetAnimation>:") {
			ReadDateFromFile(ifs, properties.useTextureSheetAnimation);
			if(properties.useTextureSheetAnimation) {
				ReadDateFromFile(ifs, properties.tileX);
				ReadDateFromFile(ifs, properties.tileY);
				MinMaxCurve::ReadMinMaxCurveFromFile(ifs, properties.texSheetAnimationCurve);
				ReadDateFromFile(ifs, properties.cycleTime);
			}
		}
		else if (token == "End") {
			break;
		}
	}
}