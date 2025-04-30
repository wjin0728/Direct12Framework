#pragma once
#include"stdafx.h"
#include<array>
#include<memory>
#include<string>
#include"CResource.h"
#include"Texture.h"
#include"Mesh.h"
#include"Material.h"

class CMaterial;
class CShader;

class CResourceManager
{
	MAKE_SINGLETON(CResourceManager)

private:
	using KeyObjMap = std::unordered_map<std::string, std::shared_ptr<CResource>>;
	std::array<KeyObjMap, RESOURCE_TYPE_COUNT> resources{};
	std::unordered_map<std::string, std::shared_ptr<class CGameObject>> prefabs{};

	std::queue<UINT> srvIdxQueue{};


public:
	void Initialize();
	void Destroy();

	template<typename T>
	std::shared_ptr<T> Load(const std::string& name, std::string_view fileName);

	template<typename T>
	bool Add(const std::shared_ptr<T>& resource);

	template<typename T>
	std::shared_ptr<T> Get(const std::string& key);

	template<typename T>
	RESOURCE_TYPE GetResourceType();

public:
	std::shared_ptr<CTexture> Create2DTexture(const std::string& name, DXGI_FORMAT format, void* data, size_t dataSize, UINT width, UINT height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor = XMFLOAT4());
	void UpdateMaterials();

	void LoadSceneResourcesFromFile(std::ifstream& ifs);
	void LoadSceneResourcesFromFile(const std::string& fileName);
	void LoadPlayerObjects();
	void LoadSkillObjects();

	void LoadPrefabFromFile(const std::string& name);

	std::unordered_map<std::string, std::shared_ptr<class CGameObject>>& GetPrefabs() { return prefabs; }

private:
	void LoadDefaultMeshes();
	void LoadDefaultTexture();
	void LoadDefaultMaterials();
	void LoadDefaultShaders();

	void MakeShadersForAllPass(const std::string& name, ShaderInfo info);

public:
	void ReleaseUploadBuffers();

	UINT GetTopSRVIndex();
	UINT GetMaterialSRVIndex();
	std::shared_ptr<class CGameObject> GetPrefab(const std::string& name)
	{
		auto itr = prefabs.find(name);
		if (itr != prefabs.end())
			return itr->second;
		return nullptr;
	}

	void ReturnSRVIndex(UINT idx) { srvIdxQueue.push(idx); }
};

template<typename T>
inline std::shared_ptr<T> CResourceManager::Load(const std::string& name, std::string_view fileName)
{
	RESOURCE_TYPE resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = resources[static_cast<UINT8>(resourceType)];

	auto itr = keyObjMap.find(name);
	if (itr != keyObjMap.end())
		return std::static_pointer_cast<T>(itr->second);

	std::shared_ptr<T> resource = std::make_shared<T>();
	resource->LoadFromFile(fileName);
	keyObjMap[name] = resource;

	if (resourceType == RESOURCE_TYPE::TEXTURE) {

	}

	return resource;
}

template<typename T>
inline bool CResourceManager::Add(const std::shared_ptr<T>& resource)
{
	if (!resource) return false;

	RESOURCE_TYPE resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = resources[static_cast<UINT>(resourceType)];

	std::string key = resource->GetName();

	auto itr = keyObjMap.find(key);
	if (itr != keyObjMap.end())
		return false;

	keyObjMap[key] = resource;

	return true;
}

template<typename T>
inline std::shared_ptr<T> CResourceManager::Get(const std::string& key)
{
	RESOURCE_TYPE resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = resources[static_cast<UINT>(resourceType)];

	auto itr = keyObjMap.find(key);
	if (itr != keyObjMap.end())
		return std::static_pointer_cast<T>(itr->second);

	return nullptr;
}

template<typename T>
inline RESOURCE_TYPE CResourceManager::GetResourceType()
{
	if (std::is_base_of_v<T, CMaterial>)
		return RESOURCE_TYPE::MATERIAL;
	else if (std::is_base_of_v<CMesh, T>)
		return RESOURCE_TYPE::MESH;
	else if (std::is_same_v<T, CShader>)
		return RESOURCE_TYPE::SHADER;
	else if (std::is_same_v<T, CTexture>)
		return RESOURCE_TYPE::TEXTURE;
}
