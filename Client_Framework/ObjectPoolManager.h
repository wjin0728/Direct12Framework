#pragma once
#include"stdafx.h"
#include"ObjectPool.h"

class CGameObject;
class CTransform;

class CObjectPoolManager
{
	MAKE_SINGLETON(CObjectPoolManager)

private:
	std::unordered_map<std::string, std::unique_ptr<CObjectPool>> mPools{};
	std::shared_ptr<CTransform> mRootTransform{};

	std::queue<UINT> cbvIdxQueue;
	std::queue<UINT> materialCBVIdxQueue; // 추가: 머티리얼 CBV 인덱스 큐
	std::queue<UINT> boneTransformIdxQueue; // 추가: 뼈 오프셋 인덱스 큐
	std::queue<UINT> lightCBVIdx;
	std::queue<UINT> UICBVIdx;

public:
	void Initialize();
	
	void CreatePool(std::unique_ptr<CGameObject>&& original, UINT objCnt);

	void PushObject(std::shared_ptr<CGameObject> object);
	std::shared_ptr<CGameObject> PopObject(const std::string& key, const std::shared_ptr<CTransform>& parent = nullptr);

	UINT GetTopCBVIndex();
	void ReturnCBVIndex(UINT idx);

	UINT GetMaterialCBVIndex();           // 추가: 머티리얼 CBV 인덱스 획득
	void ReturnMaterialCBVIndex(UINT idx); // 추가: 머티리얼 CBV 인덱스 반환

	UINT GetBoneTransformIdx();           // 추가: 뼈 오프셋 인덱스 획득
	void ReturnBoneTransformIdx(UINT idx); // 추가: 뼈 오프셋 인덱스 반환

	UINT GetLightCBVIndex();
	void ReturnLightCBVIndex(UINT idx);

	UINT GetUICBVIndex();
	void ReturnUICBVIndex(UINT idx);

	void ClearPool(const std::wstring& key);
	void ClearPools();
};

