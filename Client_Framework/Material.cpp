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
		CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->UpdateBuffer(mPoolOffset, matData, dataSize);
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

	ReadDateFromFile(inFile, token);
	std::string name = token;
	if (name == "Tree_Mat_01") {
		int i{};
	}

	std::shared_ptr<CMaterial> material{};
	std::vector<PropertyInfo> properties{};

	material = std::make_shared<CMaterial>();
	material->SetName(name);

	if(!material->uploadData) material->uploadData = new BYTE[ALIGNED_SIZE(100)];

	ReadDateFromFile(inFile, token);
	if (token == "SyntyStudios/Basic_LOD_Shader") {
		material->SetShader("Common");

		CommonProperties* data = reinterpret_cast<CommonProperties*>(material->uploadData);
		material->dataSize = sizeof(CommonProperties);

		while (true) {
			ReadDateFromFile(inFile, token);
			if (token == "<AlbedoMap>:")
			{
				data->mainTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<AlbedoColor>:")
			{
				ReadDateFromFile(inFile, data->mainColor);
			}
			else if (token == "<Smoothness>:")
			{
				ReadDateFromFile(inFile, data->smoothness);
			}
			else if (token == "<Metallic>:")
			{
				ReadDateFromFile(inFile, data->metallic);
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
			ReadDateFromFile(inFile, token);
			if (token == "<RenderMode>:") {
				float mode{};
				ReadDateFromFile(inFile, mode);
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
				ReadDateFromFile(inFile, color);
				data->mainColor = color;
			}
			else if (token == "<Smoothness>:")
			{
				ReadDateFromFile(inFile, data->smoothness);
			}
			else if (token == "<Metallic>:")
			{
				ReadDateFromFile(inFile, data->metallic);
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
				ReadDateFromFile(inFile, color);
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
			ReadDateFromFile(inFile, token);

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
				ReadDateFromFile(inFile, data->fallOff);
			}
			else if (token == "<Tiling>:")
			{
				ReadDateFromFile(inFile, data->tilling);
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
			ReadDateFromFile(inFile, token);

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
				ReadDateFromFile(inFile, data->leafNormalScale);
			}
			else if (token == "<LeafAlbedoColor>:")
			{
				Color color{};
				ReadDateFromFile(inFile, color);
				data->leafColor = color.ToVector3();
			}
			else if (token == "<LeafSmoothness>:")
			{
				ReadDateFromFile(inFile, data->leafSmoothness);
			}
			else if (token == "<LeafMetallic>:")
			{
				ReadDateFromFile(inFile, data->leafMetallic);
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
				ReadDateFromFile(inFile, data->trunkNormalScale);
			}
			else if (token == "<TrunkAlbedoColor>:")
			{
				Color color{};
				ReadDateFromFile(inFile, color);
				data->trunkColor = color.ToVector3();
			}
			else if (token == "<TrunkSmoothness>:")
			{
				ReadDateFromFile(inFile, data->trunkSmoothness);
			}
			else if (token == "<TrunkMetallic>:")
			{
				ReadDateFromFile(inFile, data->trunkMetallic);
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
			ReadDateFromFile(inFile, token);

			if (token == "<TopColor>:")
			{
				Color color{};
				ReadDateFromFile(inFile, color);
				data->topColor = color.ToVector3();
			}
			else if (token == "<BottomColor>:")
			{
				Color color{};
				ReadDateFromFile(inFile, color);
				data->bottomColor = color.ToVector3();
			}
			else if (token == "<Falloff>:")
			{
				ReadDateFromFile(inFile, data->falloff);
			}
			else if (token == "<Distance>:")
			{
				ReadDateFromFile(inFile, data->distance);
			}
			else if (token == "<Offset>:")
			{
				ReadDateFromFile(inFile, data->offset);
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
			ReadDateFromFile(inFile, token);

			if (token == "<ShallowColour>:")
			{
				ReadDateFromFile(inFile, data->shallowColor);
			}
			else if (token == "<DeepColour>:")
			{
				ReadDateFromFile(inFile, data->deepColor);
			}
			else if (token == "<VeryDeepColour>:")
			{
				ReadDateFromFile(inFile, data->veryDeepColor);
			}
			else if (token == "<FoamColor>:")
			{
				ReadDateFromFile(inFile, data->foamColor);
			}
			else if (token == "<Opacity>:")            ReadDateFromFile(inFile, data->opacity);
			else if (token == "<Smoothness>:")         ReadDateFromFile(inFile, data->smoothness);
			else if (token == "<FoamSmoothness>:")     ReadDateFromFile(inFile, data->foamSmoothness);
			else if (token == "<FoamShoreline>:")      ReadDateFromFile(inFile, data->foamShoreline);
			else if (token == "<FoamFalloff>:")        ReadDateFromFile(inFile, data->foamFalloff);
			else if (token == "<FoamSpread>:")         ReadDateFromFile(inFile, data->foamSpread);
			else if (token == "<OpacityFalloff>:")     ReadDateFromFile(inFile, data->opacityFalloff);
			else if (token == "<OpacityMin>:")         ReadDateFromFile(inFile, data->opacityMin);
			else if (token == "<ReflectionPower>:")    ReadDateFromFile(inFile, data->reflectionPower);
			else if (token == "<Depth>:")              ReadDateFromFile(inFile, data->depth);
			else if (token == "<NormalScale>:")        ReadDateFromFile(inFile, data->normalScale);
			else if (token == "<NormalTiling>:")       ReadDateFromFile(inFile, data->normalTiling);
			else if (token == "<NormalTiling2>:")      ReadDateFromFile(inFile, data->normalTiling2);
			else if (token == "<RippleSpeed>:")        ReadDateFromFile(inFile, data->rippleSpeed);
			else if (token == "<WaveDirection>:")      ReadDateFromFile(inFile, data->waveDirection);
			else if (token == "<WaveWavelength>:")     ReadDateFromFile(inFile, data->waveWavelength);
			else if (token == "<WaveAmplitude>:")      ReadDateFromFile(inFile, data->waveAmplitude);
			else if (token == "<WaveSpeed>:")          ReadDateFromFile(inFile, data->waveSpeed);
			else if (token == "<WaveFoamOpacity>:")    ReadDateFromFile(inFile, data->waveFoamOpacity);
			else if (token == "<WaveFoamSpeed>:")      ReadDateFromFile(inFile, data->waveSpeed);
			else if (token == "<WaveNoiseAmount>:")    ReadDateFromFile(inFile, data->waveNoiseAmount);
			else if (token == "<WaveNoiseScale>:")     ReadDateFromFile(inFile, data->waveNoiseScale);
			else if (token == "<RipplesNormal>:")      data->ripplesNormalIdx = GetTextureIdx(inFile);
			else if (token == "<RipplesNormal2>:")     data->ripplesNormal2Idx = GetTextureIdx(inFile);
			else if (token == "<WaveMask>:")           data->waveMaskIdx = GetTextureIdx(inFile);
			else if (token == "<FoamMask>:")           data->foamMaskIdx = GetTextureIdx(inFile);
			else if (token == "<OverallFalloff>:")     ReadDateFromFile(inFile, data->overallFalloff);
			else if (token == "<ShallowFalloff>:")     ReadDateFromFile(inFile, data->shallowFalloff);
			else if (token == "<NormalTiling2>:")      ReadDateFromFile(inFile, data->normalTiling2);
			else if (token == "<OpacityFalloff>:")     ReadDateFromFile(inFile, data->opacityFalloff);
			else if (token == "</Material>") {
				break;
			}
		}
		properties = GetPropertyInfos<WaterProperties>();
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

	ReadDateFromFile(inFile, data.size);
	ReadDateFromFile(inFile, data.splatNum);

	for (int i = 0; i < data.splatNum; i++) {
		data.alphaMapIdx[i].x = GetTextureIdx(inFile);
	}

	int splatCnt{};
	ReadDateFromFile(inFile, splatCnt);
	for (int i = 0; i < splatCnt; i++) {
		UINT idx = i / 4;
		UINT idx2 = i % 4;

		data.splats[idx].data[idx2].x = GetTextureIdx(inFile);
		data.splats[idx].data[idx2].y = GetTextureIdx(inFile);
		ReadDateFromFile(inFile, data.splats[idx].data[idx2].z);
		ReadDateFromFile(inFile, data.splats[idx].data[idx2].w);
	}
	SetShader("Terrain");
	dataSize = sizeof(TerrainData);
	
	isLoaded = true;
}




