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
	std::queue<UINT> boneTransformIdxQueue; // Ãß°¡: »À ¿ÀÇÁ¼Â ÀÎµ¦½º Å¥
	std::queue<UINT> lightCBVIdx;
	std::queue<UINT> UICBVIdx;

public:
	void Initialize();
	
	void CreatePool(std::unique_ptr<CGameObject>&& original, UINT objCnt);

	void PushObject(std::shared_ptr<CGameObject> object);
	std::shared_ptr<CGameObject> PopObject(const std::string& key, const std::shared_ptr<CTransform>& parent = nullptr);

	UINT GetTopCBVIndex();
	void ReturnCBVIndex(UINT idx);

	UINT GetBoneTransformIdx();           // Ãß°¡: »À ¿ÀÇÁ¼Â ÀÎµ¦½º È¹µæ
	void ReturnBoneTransformIdx(UINT idx); // Ãß°¡: »À ¿ÀÇÁ¼Â ÀÎµ¦½º ¹ÝÈ¯

	UINT GetLightCBVIndex();
	void ReturnLightCBVIndex(UINT idx);

	UINT GetUICBVIndex();
	void ReturnUICBVIndex(UINT idx);

	void ClearPool(const std::wstring& key);
	void ClearPools();
};

