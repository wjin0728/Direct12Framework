#include "stdafx.h"
#include "Material.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"DX12Manager.h"
#include"ObjectPoolManager.h"



CMaterial::CMaterial(const CMaterial& other)
{
	if (this == &other) return;
	dataSize = other.dataSize;
	mDirtyFrames = other.mDirtyFrames;
	mShaderName = other.mShaderName;
	isLoaded = other.isLoaded;
	mProperties = other.mProperties;
	uploadData = new BYTE[ALIGNED_SIZE(100)];
	if(other.matData) std::memcpy(uploadData, other.matData, dataSize);
	else if(other.uploadData) std::memcpy(uploadData, other.uploadData, dataSize);
	
	for (int type = PASS_TYPE::FORWARD; type < PASS_TYPE::STENCIL; type++) {
		mShaders[type] = other.mShaders[type];
	}
}

CMaterial::CMaterial(void* data, UINT dataSize) : uploadData(new BYTE[ALIGNED_SIZE(100)]), dataSize(dataSize)
{
}

CMaterial::~CMaterial()
{
	DischargeFromPool();
}

void CMaterial::Initialize(void* data, UINT dataSize)
{
	this->dataSize = dataSize;
	if(!uploadData) uploadData = new BYTE[ALIGNED_SIZE(100)];
}

void CMaterial::SetShader(const std::string& name)
{
	mShaderName = name;
	for (int type = PASS_TYPE::FORWARD; type < PASS_TYPE::STENCIL; type++) {
		std::string passType = CShader::passName[type];
		mShaders[type] = RESOURCE.Get<CShader>(name + passType);
	}
}

std::shared_ptr<CMaterial> CMaterial::Instantiate() const
{
	auto mat = std::make_shared<CMaterial>(*this);
	mat->mInstantiated = true;
	return mat;
}

void CMaterial::EnrollToPool()
{
	if (mCBVIdx >= 0) return;
	if (!isLoaded) return;
	if (dataSize == 0) return;
	mCBVIdx = INSTANCE(CObjectPoolManager).GetMaterialCBVIndex();
	mPoolOffset = mCBVIdx * ALIGNED_SIZE(100);
	if (mPoolOffset < 0) {
		std::cerr << "Failed to enroll material to pool: " + mShaderName << endl;
		return;
	}
	if (!matData) {
		matData = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->mappedData + mPoolOffset;
		if(uploadData) std::memcpy(matData, uploadData, dataSize);
	}
	mDirtyFrames = FRAME_RESOURCE_COUNT + 1; // Set dirty frames to update the material
}

void CMaterial::DischargeFromPool()
{
	if (mCBVIdx < 0) return;
	if (!isLoaded) return;
	if (!matData) return;
	INSTANCE(CObjectPoolManager).ReturnMaterialCBVIndex(mCBVIdx);
	matData = nullptr; // Reset matData to nullptr
	mCBVIdx = -1;
	mPoolOffset = -1;
	mDirtyFrames = 0; // Reset dirty frames
}

void CMaterial::Update()
{
	if (mDirtyFrames <= 0) return;
	if (!isLoaded) return;
	if (!matData) return;
	if (mCBVIdx < 0) return;

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->UpdateBuffer(mPoolOffset, matData, dataSize);

	mDirtyFrames--;
}

bool CMaterial::BindShader(PASS_TYPE passType)
{
	if (!mShaders[passType]) return false;

	mShaders[passType]->SetPipelineState(CMDLIST);
	return true;
}

void CMaterial::BindDataToShader()
{
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->BindToShader(mPoolOffset);
}

void CMaterial::CreateGPUResource()
{
	if (isLoaded) return;
	Initialize(uploadData, dataSize);
	isLoaded = true;
}

std::shared_ptr<CMaterial> CMaterial::CreateMaterialFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string token{};

	BinaryReader::ReadDateFromFile(inFile, token);
	std::string name = token;
	if (name == "Tree_Mat_01") {
		int i{};
	}

	std::shared_ptr<CMaterial> material{};
	std::vector<PropertyInfo> properties{};

	material = std::make_shared<CMaterial>();
	material->SetName(name);

	if(!material->uploadData) material->uploadData = new BYTE[ALIGNED_SIZE(100)];

	BinaryReader::ReadDateFromFile(inFile, token);
	if (token == "SyntyStudios/Basic_LOD_Shader") {
		material->SetShader("Common");

		CommonProperties* data = reinterpret_cast<CommonProperties*>(material->uploadData);
		material->dataSize = sizeof(CommonProperties);
		data->tiling = Vec2(1.0f, 1.0f);
		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);
			if (token == "<AlbedoMap>:")
			{
				data->mainTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<AlbedoColor>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->mainColor);
			}
			else if (token == "<Smoothness>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->smoothness);
			}
			else if (token == "<Metallic>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->metallic);
			}
			else if (token == "<NormalMap>:")
			{
				data->normalTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "</Material>") {

				break;
			}
		}

		properties = GetPropertyInfos<CommonProperties>();
	}
	else if (token == "Universal_Render_Pipeline/Lit") {
		LitProperties* data = reinterpret_cast<LitProperties*>(material->uploadData);
		material->dataSize = sizeof(LitProperties);

		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);
			if (token == "<RenderMode>:") {
				float mode{};
				BinaryReader::ReadDateFromFile(inFile, mode);
				if (mode == 0) material->SetShader("LitOpaque");
				else if (mode == 1) material->SetShader("LitTransparent");
			}
			if (token == "<AlbedoMap>:")
			{
				data->mainTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<AlbedoColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->mainColor = color;
			}
			else if (token == "<Smoothness>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->smoothness);
			}
			else if (token == "<Metallic>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->metallic);
			}
			else if (token == "<NormalMap>:")
			{
				data->normalTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<EmissionMap>:")
			{
				data->emissiveTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<EmissionColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->emissiveColor = color.ToVector3();
			}
			else if (token == "</Material>") {
				break;
			}
		}

		properties = GetPropertyInfos<LitProperties>();
	}
	else if (token == "SyntyStudios/Triplanar01" || token == "SyntyStudios/TriplanarBasic") {
		material->SetShader("Triplanar");
		TriplanarProperties* data = reinterpret_cast<TriplanarProperties*>(material->uploadData);
		material->dataSize = sizeof(TriplanarProperties);

		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);

			if (token == "<SidesMap>:")
			{
				data->sideTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<SidesNormalMap>:")
			{
				data->sideNormalIdx = GetTextureIdx(inFile);
			}
			else if (token == "<TopMap>:")
			{
				data->topTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<TopNormalMap>:")
			{
				data->topNormalIdx = GetTextureIdx(inFile);
			}
			else if (token == "<FallOff>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->fallOff);
			}
			else if (token == "<Tiling>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->tilling);
			}
			else if (token == "</Material>") {
				break;
			}
		}
		properties = GetPropertyInfos<TriplanarProperties>();
	}
	else if (token == "SyntyStudios/VegitationShader" || token == "SyntyStudios/VegitationShader_Basic") {
		material->SetShader("Vegitation");
		VegitationProperties* data = reinterpret_cast<VegitationProperties*>(material->uploadData);
		material->dataSize = sizeof(VegitationProperties);

		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);

			if (token == "<LeafAlbedoMap>:")
			{
				data->leafTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<LeafNormalMap>:")
			{
				data->leafNormalIdx = GetTextureIdx(inFile);
			}
			else if (token == "<LeafNormalScale>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->leafNormalScale);
			}
			else if (token == "<LeafAlbedoColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->leafColor = color.ToVector3();
			}
			else if (token == "<LeafSmoothness>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->leafSmoothness);
			}
			else if (token == "<LeafMetallic>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->leafMetallic);
			}
			else if (token == "<TrunkAlbedoMap>:")
			{
				data->trunkTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<TrunkNormalMap>:")
			{
				data->trunkNormalIdx = GetTextureIdx(inFile);
			}
			else if (token == "<TrunkNormalScale>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->trunkNormalScale);
			}
			else if (token == "<TrunkAlbedoColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->trunkColor = color.ToVector3();
			}
			else if (token == "<TrunkSmoothness>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->trunkSmoothness);
			}
			else if (token == "<TrunkMetallic>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->trunkMetallic);
			}
			else if (token == "</Material>") {
				break;
			}
		}
		properties = GetPropertyInfos<VegitationProperties>();
	}
	else if (token == "SyntyStudios/SkyboxUnlit") {
		material->SetShader("Skybox");
		SkyboxProperties* data = reinterpret_cast<SkyboxProperties*>(material->uploadData);
		material->dataSize = sizeof(SkyboxProperties);

		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);

			if (token == "<TopColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->topColor = color.ToVector3();
			}
			else if (token == "<BottomColor>:")
			{
				Color color{};
				BinaryReader::ReadDateFromFile(inFile, color);
				data->bottomColor = color.ToVector3();
			}
			else if (token == "<Falloff>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->falloff);
			}
			else if (token == "<Distance>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->distance);
			}
			else if (token == "<Offset>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->offset);
			}
			else if (token == "</Material>") {
				break;
			}
		}
	}
	else if (token == "SyntyStudios/WaterShader") {
		material->SetShader("Water");
		WaterProperties* data = reinterpret_cast<WaterProperties*>(material->uploadData);
		material->dataSize = sizeof(WaterProperties);

		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);

			if (token == "<ShallowColour>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->shallowColor);
			}
			else if (token == "<DeepColour>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->deepColor);
			}
			else if (token == "<VeryDeepColour>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->veryDeepColor);
			}
			else if (token == "<FoamColor>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->foamColor);
			}
			else if (token == "<Opacity>:")            BinaryReader::ReadDateFromFile(inFile, data->opacity);
			else if (token == "<Smoothness>:")         BinaryReader::ReadDateFromFile(inFile, data->smoothness);
			else if (token == "<FoamSmoothness>:")     BinaryReader::ReadDateFromFile(inFile, data->foamSmoothness);
			else if (token == "<FoamShoreline>:")      BinaryReader::ReadDateFromFile(inFile, data->foamShoreline);
			else if (token == "<FoamFalloff>:")        BinaryReader::ReadDateFromFile(inFile, data->foamFalloff);
			else if (token == "<FoamSpread>:")         BinaryReader::ReadDateFromFile(inFile, data->foamSpread);
			else if (token == "<OpacityFalloff>:")     BinaryReader::ReadDateFromFile(inFile, data->opacityFalloff);
			else if (token == "<OpacityMin>:")         BinaryReader::ReadDateFromFile(inFile, data->opacityMin);
			else if (token == "<ReflectionPower>:")    BinaryReader::ReadDateFromFile(inFile, data->reflectionPower);
			else if (token == "<Depth>:")              BinaryReader::ReadDateFromFile(inFile, data->depth);
			else if (token == "<NormalScale>:")        BinaryReader::ReadDateFromFile(inFile, data->normalScale);
			else if (token == "<NormalTiling>:")       BinaryReader::ReadDateFromFile(inFile, data->normalTiling);
			else if (token == "<NormalTiling2>:")      BinaryReader::ReadDateFromFile(inFile, data->normalTiling2);
			else if (token == "<RippleSpeed>:")        BinaryReader::ReadDateFromFile(inFile, data->rippleSpeed);
			else if (token == "<WaveDirection>:")      BinaryReader::ReadDateFromFile(inFile, data->waveDirection);
			else if (token == "<WaveWavelength>:")     BinaryReader::ReadDateFromFile(inFile, data->waveWavelength);
			else if (token == "<WaveAmplitude>:")      BinaryReader::ReadDateFromFile(inFile, data->waveAmplitude);
			else if (token == "<WaveSpeed>:")          BinaryReader::ReadDateFromFile(inFile, data->waveSpeed);
			else if (token == "<WaveFoamOpacity>:")    BinaryReader::ReadDateFromFile(inFile, data->waveFoamOpacity);
			else if (token == "<WaveFoamSpeed>:")      BinaryReader::ReadDateFromFile(inFile, data->waveSpeed);
			else if (token == "<WaveNoiseAmount>:")    BinaryReader::ReadDateFromFile(inFile, data->waveNoiseAmount);
			else if (token == "<WaveNoiseScale>:")     BinaryReader::ReadDateFromFile(inFile, data->waveNoiseScale);
			else if (token == "<RipplesNormal>:")      data->ripplesNormalIdx = GetTextureIdx(inFile);
			else if (token == "<RipplesNormal2>:")     data->ripplesNormal2Idx = GetTextureIdx(inFile);
			else if (token == "<WaveMask>:")           data->waveMaskIdx = GetTextureIdx(inFile);
			else if (token == "<FoamMask>:")           data->foamMaskIdx = GetTextureIdx(inFile);
			else if (token == "<OverallFalloff>:")     BinaryReader::ReadDateFromFile(inFile, data->overallFalloff);
			else if (token == "<ShallowFalloff>:")     BinaryReader::ReadDateFromFile(inFile, data->shallowFalloff);
			else if (token == "<NormalTiling2>:")      BinaryReader::ReadDateFromFile(inFile, data->normalTiling2);
			else if (token == "<OpacityFalloff>:")     BinaryReader::ReadDateFromFile(inFile, data->opacityFalloff);
			else if (token == "</Material>") {
				break;
			}
		}
		properties = GetPropertyInfos<WaterProperties>();
	}
	else if (token == "SyntyStudios/TexturePanner") {
		material->SetShader("Scrolling");
		ScrollingProperties* data = reinterpret_cast<ScrollingProperties*>(material->uploadData);
		material->dataSize = sizeof(ScrollingProperties);
		while (true) {
			BinaryReader::ReadDateFromFile(inFile, token);
			if (token == "<AlbedoMap>:")
			{
				data->albedoTextureIndex = GetTextureIdx(inFile);
			}
			else if (token == "<AlbedoColor>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->albedoTint);
			}
			else if (token == "<NormalMap>:")
			{
				data->normalTextureIndex = GetTextureIdx(inFile);
			}
			else if (token == "<EmissionMap>:")
			{
				data->emissionTextureIndex = GetTextureIdx(inFile);
			}
			else if (token == "<Smoothness>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->smoothness);
			}
			else if (token == "<Metallic>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->metallic);
			}
			else if (token == "<EmissionValue>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->emissionValue);
			}
			else if (token == "<Tiling>:")
			{
				Vec2 tiling{};
				BinaryReader::ReadDateFromFile(inFile, data->tiling);
			}
			else if (token == "<Offset>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->offset);
			}
			else if (token == "<Scroll>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->scrollSpeed);
			}
			else if (token == "</Material>") {
				break;
			}
		}
		properties = GetPropertyInfos<ScrollingProperties>();
	}
	else if (token =="SyntyStudios/CloudShader") {
		material->SetShader("Cloud");

		CloudProperties* data = reinterpret_cast<CloudProperties*>(material->uploadData);
		material->dataSize = sizeof(CloudProperties);

		data->emissiveColor = Color(1, 1, 1, 1);

		std::string token;
		while (true)
		{
			BinaryReader::ReadDateFromFile(inFile, token);

			if (token == "<EmissiveColour>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->emissiveColor);
			}
			else if (token == "<lightDirMulti>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->lightDirMultiplier);
			}
			else if (token == "<minEmit>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->minEmit);
			}
			else if (token == "<minEmit_dir>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->minEmitDir);
			}
			else if (token == "<maxEmit>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->maxEmit);
			}
			else if (token == "<DirectLight>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->directLight);
			}
			else if (token == "<lightMin>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->lightMin);
			}
			else if (token == "<lightingContrast>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->lightingContrast);
			}
			else if (token == "<lightMax>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->lightMax);
			}
			else if (token == "<WindEffect>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->windEffect);
			}
			else if (token == "<PanningSpeed>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->panningSpeed);
			}
			else if (token == "<PanningNoise>:")
			{
				data->noiseTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<WindNoiseScale>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->windNoiseScale);
			}
			else if (token == "<WindWorldScale>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->windWorldScale);
			}
			else if (token == "<X_Multiplier>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->xMultiplier);
			}
			else if (token == "<Y_Multiplier>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->yMultiplier);
			}
			else if (token == "<Z_Multiplier>:")
			{
				BinaryReader::ReadDateFromFile(inFile, data->zMultiplier);
			}
			else if (token == "</Material>")
			{
				break;
			}
		}

		properties = GetPropertyInfos<CloudProperties>();
	}
	else {
		return nullptr;
	}

	if (RESOURCE.Get<CMaterial>(name)) {
		delete[] material->uploadData;
		return nullptr;
	}
	//if (!matData) return nullptr;
	//if (dataSize == 0) return nullptr;

	for (const auto& prop : properties) {
		material->mProperties[prop.name] = prop;
	}
	return material;
}

int CMaterial::GetTextureIdx(std::ifstream& inFile)
{
	std::string name{};
	BinaryReader::ReadDateFromFile(inFile, name);

	if (name == "null") 
		return -1;

	if (RESOURCE.Get<CTexture>(name)) {
		return RESOURCE.Get<CTexture>(name)->GetSrvIndex();
	}
	std::string path = TEXTURE_PATH(name);
	auto mainTex = std::make_shared<CTexture>(name, path);
	RESOURCE.Add(mainTex);

	return mainTex->GetSrvIndex();
}

void CTerrainMaterial::Update()
{
	if (mDirtyFrames <= 0) return;
	mDirtyFrames--;
}

void CTerrainMaterial::LoadTerrainData(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string token{};

	BinaryReader::ReadDateFromFile(inFile, data.size);
	BinaryReader::ReadDateFromFile(inFile, data.splatNum);

	for (int i = 0; i < data.splatNum; i++) {
		data.alphaMapIdx[i].x = GetTextureIdx(inFile);
	}

	int splatCnt{};
	BinaryReader::ReadDateFromFile(inFile, splatCnt);
	for (int i = 0; i < splatCnt; i++) {
		UINT idx = i / 4;
		UINT idx2 = i % 4;

		data.splats[idx].data[idx2].x = GetTextureIdx(inFile);
		data.splats[idx].data[idx2].y = GetTextureIdx(inFile);
		BinaryReader::ReadDateFromFile(inFile, data.splats[idx].data[idx2].z);
		BinaryReader::ReadDateFromFile(inFile, data.splats[idx].data[idx2].w);
	}
	SetShader("Terrain");
	dataSize = sizeof(TerrainData);
	
	isLoaded = true;
}




