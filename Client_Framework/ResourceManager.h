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

struct ResourceRequest
{
	enum Type { Texture, Mesh, Shader, Material } type;
	std::string filepath;
};

class CResourceManager
{
	MAKE_SINGLETON(CResourceManager)

private:
	std::array<std::unordered_map<std::string, std::shared_ptr<CResource>>, RESOURCE_TYPE_COUNT> mResources{};
	std::unordered_map<std::string, std::shared_ptr<class CGameObject>> mPrefabs{};

	std::queue<UINT> mSrvIdxQueue{};

	std::thread mLoadThread;
	std::queue<CResource*> mGPULoadQueue;
	std::mutex mQueueMutex;

	bool mThreadRunning = false;
	bool mLoadFinished = false;

public:
	void Initialize();
	void Destroy();

	template<typename T>
	bool Add(const std::shared_ptr<T>& resource);

	template<typename T>
	std::shared_ptr<T> Get(const std::string& key);

	template<typename T>
	RESOURCE_TYPE GetResourceType();

public:
	void UpdateMaterials();

	bool LoadSceneResourcesFromFile(std::ifstream& ifs);
	bool LoadSceneResourcesFromFile(const std::string& fileName);
	bool LoadPlayerObjects();
	bool LoadEnemyObjects();
	bool LoadSkillObjects();
	bool LoadProjectileObjects();
	bool LoadLoadingScreen();

	void LoadPrefabFromFile(const std::string& name);

	std::unordered_map<std::string, std::shared_ptr<class CGameObject>>& GetPrefabs() { return mPrefabs; }

public:
	void LoadDefaultMeshes();
	void LoadDefaultTexture();
	void LoadDefaultMaterials();
	void LoadDefaultShaders();


	void MakeShadersForAllPass(const std::string& shaderName, const std::string& name, ShaderInfo info);

public:
	void ReleaseUploadBuffers();

	UINT GetTopSRVIndex();
	UINT GetMaterialSRVIndex();
	std::shared_ptr<class CGameObject> GetPrefab(const std::string& name)
	{
		auto itr = mPrefabs.find(name);
		if (itr != mPrefabs.end())
			return itr->second;
		return nullptr;
	}

	void ReturnSRVIndex(UINT idx) { mSrvIdxQueue.push(idx); }

	void BackgroundLoadingThread();
	void EnqueueRequest(CResource* req);
	void ProcessGPULoadImmediate(CResource* req); // 즉시 처리
	void ProcessGPULoadQueue(int maxPerFrame = 0); // 매 프레임 호출
	void RequestLoad();
	bool IsLoadFinished() const;
	bool IsGPULoadQueueEmpty() const { return mGPULoadQueue.empty(); }
};

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
	if (!resource->isLoaded) EnqueueRequest(resource.get());

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
