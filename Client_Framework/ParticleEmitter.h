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

	Vec3 EvaluateColor(float t) const;
	float EvaluateAlpha(float t) const;
	Color Evaluate(float t) const {
		Color color = EvaluateColor(t);
		color.w = EvaluateAlpha(t);
		return color;
	}

	static void ReadGradientFromFile(std::ifstream& ifs, Gradient& gradient) {
		using namespace BinaryReader;
		int colorKeyCount{};
		int alphaKeyCount{};

		ReadDateFromFile(ifs, colorKeyCount);
		gradient.colorKeys.resize(colorKeyCount);
		for (int i = 0; i < colorKeyCount; ++i) {
			ReadDateFromFile(ifs, gradient.colorKeys[i].time);
			Color color;
			ReadDateFromFile(ifs, color);
			gradient.colorKeys[i].color = color.ToVector3();
		}
		ReadDateFromFile(ifs, alphaKeyCount);
		gradient.alphaKeys.resize(alphaKeyCount);
		for (int i = 0; i < alphaKeyCount; ++i) {
			ReadDateFromFile(ifs, gradient.alphaKeys[i].time);
			ReadDateFromFile(ifs, gradient.alphaKeys[i].alpha);
		}
	}
};

struct CurveKeyframe {
	float time;  
	float value;
	float inTangent;
	float outTangent;
};

struct Curve {
	std::vector<CurveKeyframe> keyframes;
	float Evaluate(float t) const {
		if (keyframes.empty()) return 0.f;
		if (t <= keyframes.front().time) return keyframes.front().value;
		if (t >= keyframes.back().time) return keyframes.back().value;
		for (int i = 0; i < keyframes.size() - 1; ++i) {
			const auto& k1 = keyframes[i];
			const auto& k2 = keyframes[i + 1];
			if (t >= k1.time && t <= k2.time) {
				float segmentT = (t - k1.time) / (k2.time - k1.time);
				return SimpleMath::Hermite(
					segmentT,
					k1.value, k2.value,
					k1.outTangent * (k2.time - k1.time),
					k2.inTangent * (k2.time - k1.time));
			}
		}
		return 0.f;
	}

	static void ReadCurveFromFile(std::ifstream& ifs, Curve& curve) {
		using namespace BinaryReader;
		int keyframeCount{};
		ReadDateFromFile(ifs, keyframeCount);
		curve.keyframes.resize(keyframeCount);
		for (int i = 0; i < keyframeCount; ++i) {
			ReadDateFromFile(ifs, curve.keyframes[i].time);
			ReadDateFromFile(ifs, curve.keyframes[i].value);
			ReadDateFromFile(ifs, curve.keyframes[i].inTangent);
			ReadDateFromFile(ifs, curve.keyframes[i].outTangent);
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

struct MinMaxGradient
{
	enum class GradientType
	{
		Constant = 0,
		Gradient = 1,
		RandomBetweenTwoColors = 2, 
		RandomBetweenTwoGradients = 3
	} type = GradientType::Constant;
	std::variant<Color, Gradient, std::pair<Color, Color>, std::pair<Gradient, Gradient>> data;
	Color GetRandomColor(float t = 0) const
	{
		switch (type)
		{
		case GradientType::Constant:
			return std::get<Color>(data);
		case GradientType::Gradient:
			return std::get<Gradient>(data).Evaluate(t);
		case GradientType::RandomBetweenTwoColors:
			{
			const auto& colors = std::get<std::pair<Color, Color>>(data);
			return RandomNumberGenerator::RandFloat(0.0f, 1.0f) < 0.5f ? colors.first : colors.second;
		}
		case GradientType::RandomBetweenTwoGradients:
		{
			const auto& gradients = std::get<std::pair<Gradient, Gradient>>(data);
			return RandomNumberGenerator::RandFloat(0.0f, 1.0f) < 0.5f ? gradients.first.Evaluate(t) : gradients.second.Evaluate(t);
		}
		}
		return Color(1.0f, 1.0f, 1.0f, 1.0f);
	}
	static void ReadMinMaxGradientFromFile(std::ifstream& ifs, MinMaxGradient& gradient)
	{
		using namespace BinaryReader;
		int typeInt;
		ReadDateFromFile(ifs, typeInt);
		gradient.type = static_cast<GradientType>(typeInt);
		
		switch (gradient.type)
		{
		case GradientType::Constant:
		{
			Color constantColor;
			ReadDateFromFile(ifs, constantColor);
			gradient.data = constantColor;
		}
			break;
		case GradientType::Gradient:
		{
			Gradient gradientData;
			Gradient::ReadGradientFromFile(ifs, gradientData);
			gradient.data = gradientData;
		}
			break;
		case GradientType::RandomBetweenTwoColors:
			{
			Color color1, color2;
			ReadDateFromFile(ifs, color1);
			ReadDateFromFile(ifs, color2);
			gradient.data = std::make_pair(color1, color2);
		}
			break;
		case GradientType::RandomBetweenTwoGradients:
		{
			std::pair<Gradient, Gradient> randomGradients;
			Gradient::ReadGradientFromFile(ifs, randomGradients.first);
			Gradient::ReadGradientFromFile(ifs, randomGradients.second);
			gradient.data = randomGradients;
		}
			break;
		default:
			break;
		}
	}
};

struct ShapeModule
{
	enum class ShapeType
	{
		Sphere,
		SphereShell,
		Hemisphere,
		HemisphereShell,
		Cone,
		Box,
		Mesh,
		ConeShell,
		ConeVolume,
		ConeVolumeShell,
		Circle,
		Point
	} type = ShapeType::Cone;

	float angle = 20.0f; 
	float radius = 0.5f;
	float length = 1.0f; 

	Vec3 boxSize = { 1.0f, 1.0f, 1.0f };

	Vec3 position = { 0.0f, 0.0f, 0.0f }; // Position of the shape in world space
	Vec3 rotation = { 0.0f, 0.0f, 0.0f }; // Rotation of the shape in world space
	Vec3 scale = { 1.0f, 1.0f, 1.0f }; // Scale of the shape in world space


	Matrix transform{};

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


			return Vec3::TransformNormal(localDir, transform);
		}
		case ShapeType::ConeShell:
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
		case ShapeType::ConeVolume:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XMConvertToRadians(angle));
			Vec3 localDir{
				sinf(phi) * cosf(theta),
				sinf(phi) * sinf(theta),
				cosf(phi)
			};
			return Vec3::TransformNormal(localDir, transform);
		}
		case ShapeType::Sphere:
		{
			return Vec3::TransformNormal(RandomNumberGenerator::RandUniformVec3(), transform);
		}
		case ShapeType::Box:
		{
			Vec3 randomInBox = {
				RandomNumberGenerator::RandFloat(-boxSize.x * 0.5f, boxSize.x * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.y * 0.5f, boxSize.y * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.z * 0.5f, boxSize.z * 0.5f)
			};
			return Vec3::TransformNormal(randomInBox.GetNormalized(), transform);
		}
		case ShapeType::Point:
			return Vec3(0, 0, 1);
		case ShapeType::Hemisphere:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XM_PI * 0.5f);
			Vec3 localDir{
				sinf(phi) * cosf(theta),
				sinf(phi) * sinf(theta),
				cosf(phi)
			};
			return Vec3::TransformNormal(localDir.GetNormalized(), transform);
		}
		case ShapeType::HemisphereShell:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XM_PI * 0.5f);
			Vec3 localDir{
				sinf(phi) * cosf(theta),
				sinf(phi) * sinf(theta),
				cosf(phi)
			};
			return Vec3::TransformNormal(localDir.GetNormalized(), transform);
		}
		case ShapeType::Circle:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			Vec3 localDir = Vec3::Backward; 
			return Vec3::TransformNormal(localDir, transform);
		}
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

			return Vec3::Transform(Vec3(x, y, h), transform);
		}
		case ShapeType::ConeShell:
		{
			float angleRad = angle * degToRad;
			float height = radius / tanf(angleRad);
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float r = radius * sqrtf(RandomNumberGenerator::RandFloat(0.0f, 1.0f));
			float x = r * cosf(theta);
			float y = r * sinf(theta);
			return Vec3::Transform(Vec3(x, y, height), transform);
		}
		case ShapeType::ConeVolume:
		{
			float angleRad = XMConvertToRadians(angle);

			float r = radius * sqrt(RandomNumberGenerator::RandFloat(0.0f, 1.0f));
			float theta = RandomNumberGenerator::RandFloat(0.0f, XM_2PI);

			float x = r * cos(theta);
			float y = r * sin(theta);

			float maxZ = length * (1.0f - r / radius); 
			float z = RandomNumberGenerator::RandFloat(0.0f, maxZ);
			Vec3 localPos(x, y, z);
			return Vec3::Transform(localPos, transform);

		}
		case ShapeType::Sphere:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float randomRadius = RandomNumberGenerator::RandFloat(0.0f, 1.0f) * radius; // Random radius within the hemisphere
			Vec3 localPos{
				randomRadius * sinf(phi) * cosf(theta),
				randomRadius * sinf(phi) * sinf(theta),
				randomRadius * cosf(phi)
			};

			return Vec3::Transform(localPos, transform);
		}
		case ShapeType::Box:
		{
			Vec3 pos = Vec3(
				RandomNumberGenerator::RandFloat(-boxSize.x * 0.5f, boxSize.x * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.y * 0.5f, boxSize.y * 0.5f),
				RandomNumberGenerator::RandFloat(-boxSize.z * 0.5f, boxSize.z * 0.5f)
			);
			return Vec3::Transform(pos, transform);
		}
		case ShapeType::Point:
			return Vec3(0, 0, 0);
		case ShapeType::Hemisphere:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XM_PIDIV2);
			float randomRadius = RandomNumberGenerator::RandFloat(0.0f, 1.0f) * radius; // Random radius within the hemisphere
			Vec3 localPos{
				randomRadius * sinf(phi) * cosf(theta),
				randomRadius * sinf(phi) * sinf(theta),
				randomRadius * cosf(phi)
			};

			return Vec3::Transform(localPos, transform);
		}
		case ShapeType::HemisphereShell:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float phi = RandomNumberGenerator::RandFloat(0, XM_PIDIV2);
			Vec3 localPos{
				radius * sinf(phi) * cosf(theta),
				radius * sinf(phi) * sinf(theta),
				radius * cosf(phi)
			};

			return Vec3::Transform(localPos, transform);
		}
		case ShapeType::Circle:
		{
			float theta = RandomNumberGenerator::RandFloat(0, XM_2PI);
			float r = radius;
			float x = r * cosf(theta);
			float y = r * sinf(theta);
			return Vec3::Transform(Vec3(x, y, 0), transform);
		}
		return Vec3(0, 0, 0);
		}
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
		case ShapeType::ConeVolume:
			ReadDateFromFile(ifs, shapeModule.angle);
			ReadDateFromFile(ifs, shapeModule.radius);
			ReadDateFromFile(ifs, shapeModule.length);
			break;
		case ShapeType::Sphere:
			ReadDateFromFile(ifs, shapeModule.radius);
			break;
		case ShapeType::Hemisphere:
			ReadDateFromFile(ifs, shapeModule.radius);
			break;
		case ShapeType::Box:
			ReadDateFromFile(ifs, shapeModule.boxSize.x);
			ReadDateFromFile(ifs, shapeModule.boxSize.y);
			ReadDateFromFile(ifs, shapeModule.boxSize.z);
			break;
		case ShapeType::Circle:
			ReadDateFromFile(ifs, shapeModule.radius);
			break;
		case ShapeType::Point:
			break; 
		}
		ReadDateFromFile(ifs, shapeModule.position);
		ReadDateFromFile(ifs, shapeModule.rotation);
		ReadDateFromFile(ifs, shapeModule.scale);

		shapeModule.transform = Matrix::CreateScale(shapeModule.scale) *
			Matrix::CreateFromYawPitchRoll(shapeModule.rotation.y, shapeModule.rotation.x, shapeModule.rotation.z) *
			Matrix::CreateTranslation(shapeModule.position);
	}
};

struct Burst
{
	float time;
	float interval; 
	MinMaxCurve count;
	int cycleTime;

	static void ReadBurstFromFile(std::ifstream& ifs, Burst& burst)
	{
		using namespace BinaryReader;
		ReadDateFromFile(ifs, burst.time);
		MinMaxCurve::ReadMinMaxCurveFromFile(ifs, burst.count);
		ReadDateFromFile(ifs, burst.interval);
		ReadDateFromFile(ifs, burst.cycleTime);
	}
};

struct BurstRecord
{
	bool isActive;
	int count;
	float rate;
};

struct ParticleSpawnData
{
	float ageRate;
	float rotationSpeed;
	float startRotation;
	float startSize;
	float speed;
	Vec3 direction;
	Vec3 startLocation;
	float random;
	Color startColor;
};

struct ParticleVertex
{
	Vec3 position;
	Vec3 velocity;
	float rotation;
	Color color;
	float size;
	float distanceToCamera; 
	int albedoTexIdx;
	int frameIdx;
	int tileX;
	int tileY;
	int alignment;
};

struct ParticleProperties
{
	std::vector<Burst> bursts;
	uint32_t maxParticles;
	Vec3 gravity;
	uint32_t textureIdx;
	Vec3 emissiveColor;
	ShapeModule EmitShapeModule;
	MinMaxGradient startColorGradient;
	MinMaxCurve emitRate;
	MinMaxCurve startSizeCurve;
	MinMaxCurve startSpeedCurve;
	MinMaxCurve startRotationCurve;
	MinMaxCurve startLifetimeCurve;
	MinMaxCurve texSheetAnimationCurve;
	float duration;
	int tileX;
	int tileY;
	int cycleTime = 0; 
	int alignment = 0; // 0: View aligned, 1: World aligned, 2: Local aligned

	std::shared_ptr<MinMaxGradient> colorOverTimeGradient = nullptr;
	bool useColorOverTime = false;
	std::shared_ptr<MinMaxCurve> sizeOverTimeCurve = nullptr;
	bool useSizeOverTime = false;
	std::shared_ptr<MinMaxCurve> rotationOverTimeCurve = nullptr;
	bool useRotationOverTime = false;
	bool useVelocityOverTime = false;
	std::shared_ptr<MinMaxCurve> velocityOverTimeCurveX = nullptr;
	std::shared_ptr<MinMaxCurve> velocityOverTimeCurveY = nullptr;
	std::shared_ptr<MinMaxCurve> velocityOverTimeCurveZ = nullptr;
	bool useTextureSheetAnimation = false;

	static void ReadParticlePropertiesFromFile(std::ifstream& ifs, ParticleProperties& properties);
	
};

class CParticleEmitter
{
public:
	ParticleProperties* mParticleProperties;
    float mTimeSinceLastEmit = 0.f;
	float mTotalTime = 0.f;

	std::vector<BurstRecord> mBurstRec{};
	std::vector<ParticleSpawnData> mSpawnData;
    std::vector<ParticleMotion> mParticles;
	int mActiveParticleCount = 0;

	Vec3 mLastEmitPosW = Vec3(0, 0, 0);
	Matrix mEmitterTransform = Matrix::Identity;

	bool mIsPlaying = false;
	bool mIsPaused = false;
	bool mIsLooping = false;

public:
	CParticleEmitter() = default;
	CParticleEmitter(UINT maxParticleNum);
	~CParticleEmitter();

	void Initialize(ParticleProperties* particleProperties);
	void Release();
	int UpdateParticles(ParticleVertex* dataPtr, CCamera* camera);
	void EmitParticles();
	void CreateParticle();
	void Play(const Vec3& pos);
	void Play(const Matrix& transform) {
		SetEmitterTransform(transform);
		Play();
	}
	void Play();
	void Pause();
	void Resume();
	void Reset();
	void Stop(bool reset = true);

	bool IsEnded() const {
		if (!mParticleProperties) return true;
		return !mIsLooping && (mTotalTime >= mParticleProperties->duration) && (mActiveParticleCount == 0);
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

