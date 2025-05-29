#pragma once
#include"stdafx.h"
#include"CResource.h"
#include"FrameResource.h"
#include"Shader.h"

#define REGISTER_PROPERTY(type, field) \
	{ #field, offsetof(T, field), sizeof(((T*)0)->field), 0 }


struct PropertyInfo
{
	std::string name;
	UINT offset;
	UINT size;
	UINT type; // 0: float, 1: int, 2: Vec2, 3: Vec3, 4: Vec4, 5: Texture
};

struct CommonProperties
{
	Vec3 mainColor{};
	int mainTexIdx = -1;

	int normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	float padding{};
};

struct LitProperties
{
	Vec4 mainColor{};
	int mainTexIdx = -1;

	int normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	int emissiveTexIdx = -1;
	Vec3 emissiveColor{};
};

struct TriplanarProperties
{
	float fallOff{};
	float tilling{};
	float padding{};

	int topTexIdx = -1;
	int topNormalIdx = -1;
	int sideTexIdx = -1;
	int sideNormalIdx = -1;
};

struct VegitationProperties
{
	Vec3 leafColor{};
	float leafSmoothness{};
	float leafMetallic{};
	int leafTexIdx = -1;
	int leafNormalIdx = -1;
	float leafNormalScale{};

	Vec3 trunkColor{};
	float trunkSmoothness{};
	float trunkMetallic{};
	int trunkTexIdx = -1;
	int trunkNormalIdx = -1;
	float trunkNormalScale{};
};

struct SkyboxProperties
{
	Vec3 topColor;
	float offset;
	Vec3 bottomColor;
	float distance;

	Vec3 padding1;
	float falloff;
};

struct WaterProperties
{
	Vec4 foamColor;
	Vec4 shallowColor;
	Vec4 deepColor;
	Vec4 veryDeepColor;
	float waveSpeed;
	float smoothness;
	float foamSmoothness;
	float reflectionPower;
	float opacityFalloff;
	float normalScale;
	float normalTiling2;
	float waveFoamOpacity;
	float rippleSpeed;
	float waveNoiseScale;
	float waveNoiseAmount;
	float waveDirection;
	float waveWavelength;
	float waveAmplitude;
	float depth;
	float overallFalloff;
	float shallowFalloff;
	float foamSpread;
	float foamShoreline;
	float foamFalloff;
	float opacityMin;
	float normalTiling;
	float opacity;
	int waveMaskIdx;
	int foamMaskIdx;
	int ripplesNormalIdx;
	int ripplesNormal2Idx;
	float padding;
};

struct UIProperties
{
	Vec3 color{};
	float type{0};
	int textureIdx = -1;
};


class CMaterial : public CResource
{
protected:
	std::unique_ptr<BYTE[]> matData{};
	std::unordered_map<std::string, PropertyInfo> mProperties{};
	UINT dataSize{};

	UINT mPoolOffset{};

	std::array<std::shared_ptr<CShader>, PASS_TYPE::STENCIL> mShaders{};

	UINT mDirtyFrames{FRAME_RESOURCE_COUNT};

public:
	std::string mShaderName{};
	CMaterial() = default;
	CMaterial(void* data, UINT dataSize);
	static std::shared_ptr<CMaterial> CreateMaterialFromFile(std::ifstream& inFile);

	void Initialize(void* data, UINT dataSize);
	void SetShader(const std::string& name);
	virtual void Update();
	virtual bool BindShader(PASS_TYPE passType);
	virtual void BindDataToShader();
	virtual void CreateGPUResource();
	virtual void ReleaseUploadBuffer() {}

	std::shared_ptr<CShader> GetShader(PASS_TYPE passType) const
	{
		return mShaders[passType];
	}

	template<typename T>
	void SetProperty(const std::string& name, const T& value)
	{
		auto it = mProperties.find(name);
		if (it != mProperties.end())
		{
			UINT offset = it->second.offset;
			if (sizeof(T) == dataSize)
			{
				memcpy(matData.get() + offset, &value, dataSize);
				mDirtyFrames = FRAME_RESOURCE_COUNT;
			}
		}
	}
	template<typename T>
	T GetProperty(const std::string& name) const
	{
		auto it = mProperties.find(name);
		if (it != mProperties.end())
		{
			UINT offset = it->second.offset;
			if (sizeof(T) == dataSize)
			{
				T value{};
				memcpy(&value, matData.get() + offset, sizeof(T));
				return value;
			}
		}
		return T{};
	}

protected:
	static int GetTextureIdx(std::ifstream& inFile);
};

struct alignas(16) SplatData
{
	Vec4 data[4];
};

struct alignas(16) TerrainData
{
	Vec3 size = Vec3::One;
	float yOffset{};

	int heightMapIdx = -1;
	int splatNum;
	Vec2 heightMapResolution;

	SplatData splats[TERRAIN_SPLAT_COUNT];

	Vec4 alphaMapIdx[TERRAIN_SPLAT_COUNT];
};

class CTerrainMaterial : public CMaterial
{
public:
	TerrainData data;

public:
	CTerrainMaterial() = default;
	virtual void Update();

	void LoadTerrainData(std::ifstream& inFile);
	Vec3 GetSize() const { return data.size; }

public:

};



template<typename T>
std::vector<PropertyInfo> GetPropertyInfos()
{
	return {};
}

#pragma once

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<CommonProperties>()
{
	using T = CommonProperties;
	return {
		REGISTER_PROPERTY(T, mainColor),
		REGISTER_PROPERTY(T, mainTexIdx),
		REGISTER_PROPERTY(T, normalTexIdx),
		REGISTER_PROPERTY(T, smoothness),
		REGISTER_PROPERTY(T, metallic)
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<LitProperties>()
{
	using T = LitProperties;
	return {
		REGISTER_PROPERTY(T, mainColor),
		REGISTER_PROPERTY(T, mainTexIdx),
		REGISTER_PROPERTY(T, normalTexIdx),
		REGISTER_PROPERTY(T, smoothness),
		REGISTER_PROPERTY(T, metallic),
		REGISTER_PROPERTY(T, emissiveTexIdx),
		REGISTER_PROPERTY(T, emissiveColor)
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<TriplanarProperties>()
{
	using T = TriplanarProperties;
	return {
		REGISTER_PROPERTY(T, fallOff),
		REGISTER_PROPERTY(T, tilling),
		REGISTER_PROPERTY(T, padding),
		REGISTER_PROPERTY(T, topTexIdx),
		REGISTER_PROPERTY(T, topNormalIdx),
		REGISTER_PROPERTY(T, sideTexIdx),
		REGISTER_PROPERTY(T, sideNormalIdx)
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<VegitationProperties>()
{
	using T = VegitationProperties;
	return {
		REGISTER_PROPERTY(T, leafColor),
		REGISTER_PROPERTY(T, leafSmoothness),
		REGISTER_PROPERTY(T, leafMetallic),
		REGISTER_PROPERTY(T, leafTexIdx),
		REGISTER_PROPERTY(T, leafNormalIdx),
		REGISTER_PROPERTY(T, leafNormalScale),
		REGISTER_PROPERTY(T, trunkColor),
		REGISTER_PROPERTY(T, trunkSmoothness),
		REGISTER_PROPERTY(T, trunkMetallic),
		REGISTER_PROPERTY(T, trunkTexIdx),
		REGISTER_PROPERTY(T, trunkNormalIdx),
		REGISTER_PROPERTY(T, trunkNormalScale)
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<SkyboxProperties>()
{
	using T = SkyboxProperties;
	return {
		REGISTER_PROPERTY(T, topColor),
		REGISTER_PROPERTY(T, offset),
		REGISTER_PROPERTY(T, bottomColor),
		REGISTER_PROPERTY(T, distance),
		REGISTER_PROPERTY(T, falloff),
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<WaterProperties>()
{
	using T = WaterProperties;
	return {
		REGISTER_PROPERTY(T, foamColor),
		REGISTER_PROPERTY(T, shallowColor),
		REGISTER_PROPERTY(T, deepColor),
		REGISTER_PROPERTY(T, veryDeepColor),
		REGISTER_PROPERTY(T, waveSpeed),
		REGISTER_PROPERTY(T, smoothness),
		REGISTER_PROPERTY(T, foamSmoothness),
		REGISTER_PROPERTY(T, reflectionPower),
		REGISTER_PROPERTY(T, opacityFalloff),
		REGISTER_PROPERTY(T, normalScale),
		REGISTER_PROPERTY(T, normalTiling2),
		REGISTER_PROPERTY(T, waveFoamOpacity),
		REGISTER_PROPERTY(T, rippleSpeed),
		REGISTER_PROPERTY(T, waveNoiseScale),
		REGISTER_PROPERTY(T, waveNoiseAmount),
		REGISTER_PROPERTY(T, waveDirection),
		REGISTER_PROPERTY(T, waveWavelength),
		REGISTER_PROPERTY(T, waveAmplitude),
		REGISTER_PROPERTY(T, depth),
		REGISTER_PROPERTY(T, overallFalloff),
		REGISTER_PROPERTY(T, shallowFalloff),
		REGISTER_PROPERTY(T, foamSpread),
		REGISTER_PROPERTY(T, foamShoreline),
		REGISTER_PROPERTY(T, foamFalloff),
		REGISTER_PROPERTY(T, opacityMin),
		REGISTER_PROPERTY(T, normalTiling),
		REGISTER_PROPERTY(T, opacity),

		REGISTER_PROPERTY(T, waveMaskIdx),
		REGISTER_PROPERTY(T, foamMaskIdx),
		REGISTER_PROPERTY(T, ripplesNormalIdx),
		REGISTER_PROPERTY(T, ripplesNormal2Idx)
	};
}