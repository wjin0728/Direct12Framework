#include "stdafx.h"
#include "Material.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"DX12Manager.h"




CMaterial::CMaterial(void* data, UINT dataSize) : matData(new BYTE[dataSize]), dataSize(dataSize)
{
	std::memcpy(matData.get(), data, dataSize);
	mPoolOffset = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->AddData(matData.get(), dataSize);
}

void CMaterial::Initialize(void* data, UINT dataSize)
{
	this->dataSize = dataSize;
	if (!matData) {
		matData.reset(new BYTE[dataSize]);
		std::memcpy(matData.get(), data, dataSize);
	}
	mPoolOffset = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->AddData(matData.get(), dataSize);
}

void CMaterial::SetShader(const std::string& name)
{
	mShaderName = name;
	for (int type = PASS_TYPE::FORWARD; type < PASS_TYPE::STENCIL; type++) {
		std::string passType = CShader::passName[type];
		mShaders[type] = RESOURCE.Get<CShader>(name + passType);
	}
}

void CMaterial::Update()
{
	if (mDirtyFrames <= 0) return;

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->UpdateBuffer(mPoolOffset, matData.get(), dataSize);

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
	Initialize(matData.get(), dataSize);
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

	material = std::make_shared<CMaterial>();
	material->SetName(name);

	ReadDateFromFile(inFile, token);
	if (token == "SyntyStudios/Basic_LOD_Shader") {
		material->SetShader("Common");
		
		material->matData = std::make_unique<BYTE[]>(sizeof(CommonProperties));
		CommonProperties* data = reinterpret_cast<CommonProperties*>(material->matData.get());
		material->dataSize = sizeof(CommonProperties);

		while (true) {
			ReadDateFromFile(inFile, token);

			if (token == "<AlbedoMap>:")
			{
				data->mainTexIdx = GetTextureIdx(inFile);
			}
			else if (token == "<AlbedoColor>:")
			{
				Color color{};
				ReadDateFromFile(inFile, color);
				data->mainColor = color.ToVector3();
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
	}
	else if (token == "Universal_Render_Pipeline/Lit") {
		material->matData = std::make_unique<BYTE[]>(sizeof(LitProperties));
		LitProperties* data = reinterpret_cast<LitProperties*>(material->matData.get());
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
	}
	else if (token == "SyntyStudios/Triplanar01" || token == "SyntyStudios/TriplanarBasic") {
		material->SetShader("Triplanar");
		material->matData = std::make_unique<BYTE[]>(sizeof(TriplanarProperties));
		TriplanarProperties* data = reinterpret_cast<TriplanarProperties*>(material->matData.get());
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
	}
	else if (token == "SyntyStudios/VegitationShader" || token == "SyntyStudios/VegitationShader_Basic") {
		material->SetShader("Vegitation");
		material->matData = std::make_unique<BYTE[]>(sizeof(VegitationProperties));
		VegitationProperties* data = reinterpret_cast<VegitationProperties*>(material->matData.get());
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
	}
	else if (token == "SyntyStudios/SkyboxUnlit") {
		material->SetShader("Skybox");
		material->matData = std::make_unique<BYTE[]>(sizeof(SkyboxProperties));
		SkyboxProperties* data = reinterpret_cast<SkyboxProperties*>(material->matData.get());
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
	else {
		return nullptr;
	}

	if (RESOURCE.Get<CMaterial>(name)) 
		return nullptr;
	//if (!matData) return nullptr;
	//if (dataSize == 0) return nullptr;
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

	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->UpdateBuffer(mPoolOffset, &data, dataSize);

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


	Initialize(&data, sizeof(TerrainData));
	SetShader("Terrain");

	mDirtyFrames = FRAME_RESOURCE_COUNT + 1;
}




