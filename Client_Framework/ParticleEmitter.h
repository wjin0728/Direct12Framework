#pragma once
#include"stdafx.h"
#include"UploadBuffer.h"
#include"Component.h"
#include <variant>

struct GradientColorKey {
	float time;  
	Vec3 color; 
};

struct GradientAlphaKey {
	float time;
	float alpha;
};

struct Gradient {
	std::vector<GradientColorKey> colorKeys;
	std::vector<GradientAlphaKey> alphaKeys;

	Vec3 EvaluateColor(float t);
	float EvaluateAlpha(float t);
	Color Evaluate(float t) {
		Color color = EvaluateColor(t);
		color.w = EvaluateAlpha(t);
		return color;
	}

	static void ReadGradientFromFile(std::ifstream& ifs, Gradient& gradient) {
		using namespace BinaryReader;
		size_t colorKeyCount, alphaKeyCount;

		ReadDateFromFile(ifs, colorKeyCount);
		gradient.colorKeys.resize(colorKeyCount);
		for (size_t i = 0; i < colorKeyCount; ++i) {
			ReadDateFromFile(ifs, gradient.colorKeys[i].time);
			Color color;
			ReadDateFromFile(ifs, color);
			gradient.colorKeys[i].color = color.ToVector3();
		}
		ReadDateFromFile(ifs, alphaKeyCount);
		gradient.alphaKeys.resize(alphaKeyCount);
		for (size_t i = 0; i < alphaKeyCount; ++i) {
			ReadDateFromFile(ifs, gradient.alphaKeys[i].time);
			ReadDateFromFile(ifs, gradient.alphaKeys[i].alpha);
		}
	}
};

struct CurveKeyframe {
	float time;  
	float value;
	float inTangent;
};

struct Curve {
	std::vector<CurveKeyframe> keyframes;
	float Evaluate(float t) const {
		if (keyframes.empty()) return 0.f;
		if (t <= keyframes.front().time) return keyframes.front().value;
		if (t >= keyframes.back().time) return keyframes.back().value;
		for (size_t i = 0; i < keyframes.size() - 1; ++i) {
			const auto& k1 = keyframes[i];
			const auto& k2 = keyframes[i + 1];
			if (t >= k1.time && t <= k2.time) {
				return SimpleMath::Hermite(t, k1.time, k1.value, k1.inTangent, k2.time, k2.value, k2.inTangent);
			}
		}
		return 0.f;
	}

	static void ReadCurveFromFile(std::ifstream& ifs, Curve& curve) {
		using namespace BinaryReader;
		size_t keyframeCount;
		ReadDateFromFile(ifs, keyframeCount);
		curve.keyframes.resize(keyframeCount);
		for (size_t i = 0; i < keyframeCount; ++i) {
			ReadDateFromFile(ifs, curve.keyframes[i].time);
			ReadDateFromFile(ifs, curve.keyframes[i].value);
			ReadDateFromFile(ifs, curve.keyframes[i].inTangent);
		}
	}
};

struct MinMaxCurve
{
	enum class CurveType
	{
		Constant = 0,
		Curve = 1,
		RandomBetweenTwoCurves = 2,
		RandomBetweenTwoConstants = 3
	} type = CurveType::Constant;	

	struct RandomConstants
	{
		float min = 0.0f;
		float max = 1.0f;
	};
	struct RandomCurves
	{
		Curve minCurve;
		Curve maxCurve;
	};

	std::variant<float, Curve, RandomConstants, RandomCurves> data;

	float GetRandomValue(float t = 0) const
	{
		switch (type)
		{
		case CurveType::Constant:
			return std::get<float>(data);
		case CurveType::Curve:
			return std::get<Curve>(data).Evaluate(t);
		case CurveType::RandomBetweenTwoConstants:
		{
			const auto& rc = std::get<RandomConstants>(data);
			return RandomNumberGenerator::RandFloat(rc.min, rc.max);
		}
		case CurveType::RandomBetweenTwoCurves:
		{
			const auto& rc = std::get<RandomCurves>(data);
			float min = rc.minCurve.Evaluate(t);
			float max = rc.maxCurve.Evaluate(t);
			return RandomNumberGenerator::RandFloat(min, max);
		}
		}
		return 0.0f;
	}

	static void ReadMinMaxCurveFromFile(std::ifstream& ifs, MinMaxCurve& curve)
	{
		using namespace BinaryReader;
		int typeInt;
		ReadDateFromFile(ifs, typeInt);
		curve.type = static_cast<CurveType>(typeInt);
		switch (curve.type)
		{
		case CurveType::Constant:
		{
			float constantValue;
			ReadDateFromFile(ifs, constantValue);
			curve.data = constantValue;
			break;
		}
		case CurveType::Curve:
		{
			Curve curveData;
			Curve::ReadCurveFromFile(ifs, curveData);
			curve.data = curveData;
			break;
		}
		case CurveType::RandomBetweenTwoConstants:
		{
			RandomConstants randomConstants;
			ReadDateFromFile(ifs, randomConstants.min);
			ReadDateFromFile(ifs, randomConstants.max);
			curve.data = randomConstants;
			break;
		}
		case CurveType::RandomBetweenTwoCurves:
		{
			RandomCurves randomCurves;
			Curve::ReadCurveFromFile(ifs, randomCurves.minCurve);
			Curve::ReadCurveFromFile(ifs, randomCurves.maxCurve);
			curve.data = randomCurves;
			break;
		}
		}
	}
};

struct ShapeModule
{
	enum class ShapeType
	{
		Cone,
		Sphere,
		Box,
		Point
	} type = ShapeType::Cone;

	float angle = 20.0f; 
	float radius = 0.5f;
	float length = 1.0f; 

	Vec3 boxSize = { 1.0f, 1.0f, 1.0f };

	float sphereRadius = 1.0f;

	Vec3 GetRandomDirection() const
	{
		switch (type)
		{
		case ShapeType::Cone:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XMConvertToRadians(angle));
			Vec3 localDir{
				sinf(phi) * cosf(theta),
				sinf(phi) * sinf(theta),
				cosf(phi)
			};
			return localDir;
		}
		case ShapeType::Sphere:
		{
			return RandomNumberGenerator::RandUniformVec3();
		}
		case ShapeType::Box:
		{
			Vec3 randomInBox = {
				RandomNumberGenerator::RandFloat(-boxSize.x * 0.5f, boxSize.x * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.y * 0.5f, boxSize.y * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.z * 0.5f, boxSize.z * 0.5f)
			};
			return randomInBox.GetNormalized();
		}
		case ShapeType::Point:
			return Vec3(0, 0, 1);
		}
	}
	Vec3 GetRandomPosition() const
	{
		switch (type)
		{
		case ShapeType::Cone:
		{
			float angleRad = angle * degToRad;

			float height = radius / tanf(angleRad);

			float h = height * cbrtf(RandomNumberGenerator::RandFloat(0.0f, 1.0f));
			float r_at_h = radius * (h / height);

			float theta = RandomNumberGenerator::RandFloat(0.0f, 2.0f * 3.1415926535f);
			float r = r_at_h * sqrtf(RandomNumberGenerator::RandFloat(0.0f, 1.0f));
			float x = r * cosf(theta);
			float y = r * sinf(theta);

			return Vec3(x, y, h);
		}
		case ShapeType::Sphere:
			return RandomNumberGenerator::RandUniformVec3() * RandomNumberGenerator::RandFloat(0, sphereRadius);
		case ShapeType::Box:
			return Vec3(
				RandomNumberGenerator::RandFloat(-boxSize.x * 0.5f, boxSize.x * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.y * 0.5f, boxSize.y * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.z * 0.5f, boxSize.z * 0.5f)
			);
		case ShapeType::Point:
			return Vec3(0, 0, 0); // Point emits from a single point
		}
		return Vec3(0, 0, 0);
	}

	static void ReadShapeModuleFromFile(std::ifstream& ifs, ShapeModule& shapeModule)
	{
		using namespace BinaryReader;
		int typeInt;
		ReadDateFromFile(ifs, typeInt);
		shapeModule.type = static_cast<ShapeType>(typeInt);
		switch (shapeModule.type)
		{
		case ShapeType::Cone:
			ReadDateFromFile(ifs, shapeModule.angle);
			ReadDateFromFile(ifs, shapeModule.radius);
			ReadDateFromFile(ifs, shapeModule.length);
			break;
		case ShapeType::Sphere:
			ReadDateFromFile(ifs, shapeModule.sphereRadius);
			break;
		case ShapeType::Box:
			ReadDateFromFile(ifs, shapeModule.boxSize.x);
			ReadDateFromFile(ifs, shapeModule.boxSize.y);
			ReadDateFromFile(ifs, shapeModule.boxSize.z);
			break;
		case ShapeType::Point:
			break; 
		}
	}
};

struct ParticleVertex
{
	Vec3 position;
	float rotation;
	Color color;
	float size;
	int albedoTexIdx;
};

struct ParticleProperties
{
	uint32_t maxParticles;
	Vec3 gravity;
	uint32_t textureIdx;
	Vec3 emissiveColor;
	ShapeModule EmitShapeModule;
	Gradient startColorGradient;
	MinMaxCurve emitRate;
	MinMaxCurve startSizeCurve;
	MinMaxCurve startSpeedCurve;
	MinMaxCurve startRotationCurve;
	MinMaxCurve startLifetimeCurve;
	float duration;

	std::shared_ptr<Gradient> colorOverTimeGradient = nullptr;
	bool useColorOverTime = false;
	std::shared_ptr<MinMaxCurve> sizeOverTimeCurve = nullptr;
	bool useSizeOverTime = false;
	std::shared_ptr<MinMaxCurve> rotationOverTimeCurve = nullptr;
	bool useRotationOverTime = false;
	bool useVelocityOverTime = false;

	static void ReadParticlePropertiesFromFile(std::ifstream& ifs, ParticleProperties& properties);
	
};

class CParticleEmitter
{
public:
	ParticleProperties* mParticleProperties;
    float mTimeSinceLastEmit = 0.f;
	float mTotalTime = 0.f;

	std::vector<ParticleSpawnData> mSpawnData;
    std::vector<ParticleMotion> mParticles;

	Vec3 mLastEmitPosW = Vec3(0, 0, 0);
	Matrix mEmitterTransform = Matrix::Identity;

	bool mIsPlaying = true;
	bool mIsPaused = false;
	bool mIsActive = true;
	bool mIsLooping = false;

public:
	CParticleEmitter() = default;
	CParticleEmitter(UINT maxParticleNum);
	~CParticleEmitter();

	void Initialize(ParticleProperties* particleProperties);
	void Release();
	int UpdateParticles(ParticleVertex* dataPtr);
	void EmitParticles();
	void Play(const Vec3& pos);
	void Pause();
	void Resume();
	void Reset();

	bool IsEnded() const {
		return !mIsLooping && (mTotalTime >= mParticleProperties->duration);
	}

	void SetEmitterLocation(const Vec3& location) { 
		mLastEmitPosW = location;
		mEmitterTransform._41 = location.x;
		mEmitterTransform._42 = location.y;
		mEmitterTransform._43 = location.z;
	}
	void SetEmitterTransform(const Matrix& transform) { 
		mEmitterTransform = transform; 
		mLastEmitPosW = Vec3(transform._41, transform._42, transform._43);
	}

	int GetParticleCount() const { return mParticleProperties->maxParticles; }
};

