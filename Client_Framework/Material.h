#pragma once
#include"stdafx.h"
#include"CResource.h"
#include"FrameResource.h"
#include"Shader.h"

#define REGISTER_PROPERTY(type, field) \
	{ #field, offsetof(type, field), sizeof(((type*)0)->field), 0 }

#define REGISTER_PROPERTY_NAME(type, field, fieldName) \
	{ #fieldName, offsetof(type, field), sizeof(((type*)0)->field), 0 }


struct PropertyInfo
{
	std::string name;
	UINT offset;
	UINT size;
	UINT type; // 0: float, 1: int, 2: Vec2, 3: Vec3, 4: Vec4, 5: Texture
};

struct CommonProperties
{
	Vec4 mainColor{};
	UINT mainTexIdx = -1;

	UINT normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	Vec4 vec4Data0{}; 
	Vec4 vec4Data1{};
	float fData0{};
	float fData1{};
	Vec2 tiling{1.f,1.f}; 
	int iData0{};
	int iData1{};
	int iData2{};
	int iData3{};
};

struct LitProperties
{
	Vec4 mainColor{};
	UINT mainTexIdx = -1;

	UINT normalTexIdx = -1;
	float smoothness{};
	float metallic{};
	UINT emissiveTexIdx = -1;
	Vec3 emissiveColor{};
};

struct TriplanarProperties
{
	float fallOff{};
	float tilling{};
	Vec2 padding{};

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

struct PortalProperties
{
	float waveAmplitude;
	float waveDirection; 
	float waveSpeed; 
	float waveWavelength; 
	float waveNoiseScale; 
	float waveNoiseAmount; 
	UINT mainTextureIndex;
	UINT waveMask; 
};

struct ScrollingProperties
{
    Vec4 albedoTint;  
	UINT albedoTextureIndex;  
    UINT normalTextureIndex;  
    UINT emissionTextureIndex;
    float emissionValue;      

	Vec2 scrollSpeed{ 0.f, 0.f }; 
    float metallic;           
    float smoothness;         

	Vec2 tiling{ 1.f, 1.f };
	Vec2 offset{ 0.f, 0.f };

};

struct CloudProperties
{
	Vec4 emissiveColor;
	float lightDirMultiplier;
	float minEmit;
	float minEmitDir;
	float maxEmit;

	float directLight;
	float lightMin;
	float lightingContrast;
	float lightMax;

	float windEffect;
	float panningSpeed;
	float windNoiseScale;
	float windWorldScale;

	float xMultiplier = 1.0f;
	float yMultiplier = 1.0f;
	float zMultiplier = 1.0f;

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
	BYTE* matData{};
	BYTE* uploadData{};
	std::unordered_map<std::string, PropertyInfo> mProperties{};
	UINT dataSize{};

	int mCBVIdx{ -1 }; 
	UINT mPoolOffset{};

	std::array<std::shared_ptr<CShader>, PASS_TYPE::STENCIL> mShaders{};

	UINT mDirtyFrames{FRAME_RESOURCE_COUNT};

	bool mInstantiated{ false };

public:
	std::string mShaderName{};
	CMaterial() = default;
	CMaterial(const std::string& name) : CResource(name, RESOURCE_TYPE::MATERIAL) {}
	CMaterial(const CMaterial& other);
	CMaterial(void* data, UINT dataSize);
	virtual ~CMaterial();

	static std::shared_ptr<CMaterial> CreateMaterialFromFile(std::ifstream& inFile);

	virtual void Initialize(void* data, UINT dataSize);
	void SetShader(const std::string& name);
	std::shared_ptr<CMaterial> Instantiate() const;

	void EnrollToPool();
	void DischargeFromPool();
	virtual void Update();
	virtual bool BindShader(PASS_TYPE passType);
	virtual void BindDataToShader();
	virtual void CreateGPUResource();
	virtual void ReleaseUploadBuffer() {}

	std::shared_ptr<CShader> GetShader(PASS_TYPE passType) const
	{
		return mShaders[passType];
	}

	void AddPropertyKey(const PropertyInfo& propertyInfo)
	{
		mProperties[propertyInfo.name] = propertyInfo;
	}
	void AddPropertyKey(const std::vector<PropertyInfo>& propertyInfos)
	{
		for (const auto& propertyInfo : propertyInfos)
		{
			mProperties[propertyInfo.name] = propertyInfo;
		}
	}

	template<typename T>
	void SetProperty(const std::string& name, const T& value)
	{
		auto it = mProperties.find(name);
		if (it != mProperties.end())
		{
			UINT offset = it->second.offset;
			if (sizeof(T) == it->second.size)
			{
				if (!matData)
				{
					memcpy(uploadData + offset, &value, sizeof(T));
				}
				else memcpy(matData + offset, &value, it->second.size);
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
			if (sizeof(T) == it->second.size)
			{
				T value{};
				if (!matData)
				{
					memcpy(&value, uploadData + offset, sizeof(T));
				}
				else memcpy(&value, matData + offset, sizeof(T));
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
	virtual ~CTerrainMaterial() = default;
	virtual void Update();

	void Initialize()
	{
		uploadData = new BYTE[ALIGNED_SIZE(100)];

		std::memcpy(uploadData, &data, sizeof(TerrainData));

		mDirtyFrames = FRAME_RESOURCE_COUNT + 1;
	}
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
inline std::vector<PropertyInfo> GetPropertyInfos<ScrollingProperties>()
{
	using T = ScrollingProperties;
	return {
		REGISTER_PROPERTY(T, albedoTint),
		REGISTER_PROPERTY(T, albedoTextureIndex),
		REGISTER_PROPERTY(T, normalTextureIndex),
		REGISTER_PROPERTY(T, emissionTextureIndex),
		REGISTER_PROPERTY(T, emissionValue),
		REGISTER_PROPERTY(T, metallic),
		REGISTER_PROPERTY(T, smoothness),
		REGISTER_PROPERTY(T, tiling),
		REGISTER_PROPERTY(T, offset),
		REGISTER_PROPERTY(T, scrollSpeed)
	};
}

template<>
inline std::vector<PropertyInfo> GetPropertyInfos<CommonProperties>()
{
	using T = CommonProperties;
	return {
		REGISTER_PROPERTY(T, mainColor),
		REGISTER_PROPERTY(T, mainTexIdx),
		REGISTER_PROPERTY(T, normalTexIdx),
		REGISTER_PROPERTY(T, smoothness),
		REGISTER_PROPERTY(T, metallic),
		REGISTER_PROPERTY(T, tiling)
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