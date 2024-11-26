#include "stdafx.h"
#include "Material.h"
#include"FrameResource.h"
#include"ResourceManager.h"
#include"DX12Manager.h"

CMaterial::CMaterial()
{
	mSrvIdx = INSTANCE(CResourceManager).GetMaterialSRVIndex();
}

void CMaterial::Update()
{
	if (mDirtyFramesNum <= 0) {
		return;
	}

	CBMaterialDate materialDate{};
	materialDate.albedoColor = mAlbedoColor;
	materialDate.emissiveColor = mEmissiveColor;
	materialDate.specularColor = mSpecularColor;
	materialDate.diffuseMapIdx = mDiffuseMapIdx;
	materialDate.normalMapIdx = mNormalMapIdx;

	UPLOADBUFFER(STRUCTED_BUFFER_TYPE::MATERIAL)->CopyData(&materialDate, mSrvIdx);

	mDirtyFramesNum--;
}

std::shared_ptr<CMaterial> CMaterial::CreateMaterialFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;

	std::string token{};
	ReadDateFromFile(inFile, token);

	std::wstring name = stringToWstring(token);

	std::shared_ptr<CMaterial> material;
	if (!(material = RESOURCE.Get<CMaterial>(name))) {
		material = std::make_shared<CMaterial>();
		material->SetName(name);
		RESOURCE.Add(material);
	}

	while (true)
	{
		ReadDateFromFile(inFile, token);

		if (token == "<AlbedoColor>:")
		{
			ReadDateFromFile(inFile, material->mAlbedoColor);
		}
		else if (token == "<EmissiveColor>:")
		{
			ReadDateFromFile(inFile, material->mEmissiveColor);
		}
		else if (token == "<SpecularColor>:")
		{
			ReadDateFromFile(inFile, material->mSpecularColor);
		}
		else if (token == "<Glossiness>:")
		{
			float glossiness{};
			ReadDateFromFile(inFile, glossiness);
			material->mSpecularColor.A(glossiness);
		}
		else if (token == "<Smoothness>:")
		{
			float Smoothness{};
			ReadDateFromFile(inFile, Smoothness);
			material->mSpecularColor.A(Smoothness);
		}
		else if (token == "<Metallic>:") {
			float metallic{};
			ReadDateFromFile(inFile, metallic);

			material->mFresnelR0 = Vec3::Lerp(material->mFresnelR0, material->mAlbedoColor.ToVector3(), metallic);
		}
		else if (token == "<SpecularHighlight>:") {
			float specularHighlight{};
			ReadDateFromFile(inFile, specularHighlight);
		}
		else if (token == "<GlossyReflection>:") {
			float glossyReflection{};
			ReadDateFromFile(inFile, glossyReflection);
		}
		else if (token == "<AlbedoMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
			std::wstring texName = stringToWstring(token);
			auto texture = RESOURCE.Load<CTexture>(texName, L"Resources\\Textures\\" + texName + L".dds");
			texture->CreateSRV();

			material->mDiffuseMapIdx = texture->GetSrvIndex();
		}
		else if (token == "<SpecularMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
		}
		else if (token == "<MetallicMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
		}
		else if (token == "<NormalMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());

			std::wstring texName = stringToWstring(token);
			auto texture = RESOURCE.Load<CTexture>(texName, L"Resources\\Textures\\" + texName + L".dds");
			texture->CreateSRV();

			material->mNormalMapIdx = texture->GetSrvIndex();
		}
		else if (token == "<EmissionMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
		}
		else if (token == "<DetailAlbedoMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
		}
		else if (token == "<DetailNormalMap>:") {
			ReadDateFromFile(inFile, token);
			if (token == "null") 
				continue;
			if (token[0] == '@')
				token.erase(token.begin());
		}
		else if (token == "</Materials>" || token == "<Material>:") {
			break;
		}
	}

	return material;
}

void CTerrainMaterial::Update()
{
	if (mDirtyFramesNum <= 0) {
		return;
	}

	mDirtyFramesNum--;
}
