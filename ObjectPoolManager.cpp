#include "stdafx.h"
#include "ObjectPoolManager.h"
#include"GameObject.h"
#include"Transform.h"
#include"UploadBuffer.h"

void CObjectPoolManager::Initialize()
{
	for (UINT i = 0; i < OBJECT_COUNT; i++) {
		cbvIdxQueue.push(i);
	}
}

void CObjectPoolManager::CreatePool(std::unique_ptr<CGameObject>&& original, UINT objCnt)
{
	const std::wstring& tag = original->GetTag();

	if (!mPools.contains(tag)) {
		mPools[tag] = std::make_unique<CObjectPool>();
		mPools[tag]->Initialize(std::move(original), objCnt);
	}
}

void CObjectPoolManager::PushObject(std::shared_ptr<CGameObject> object)
{
	if (!object) {
		return;
	}

	const std::wstring& tag = object->GetTag();

	if (mPools.contains(tag)) {
		mPools[tag]->PushObject(object);
	}
}

std::shared_ptr<CGameObject> CObjectPoolManager::PopObject(const std::wstring& key, const const std::shared_ptr<CTransform>& parent)
{
	auto itr = mPools.find(key);
	if (itr == mPools.end()) {
		return nullptr;
	}

	return itr->second->PopObject(parent);
}

UINT CObjectPoolManager::GetTopCBVIndex()
{
	UINT idx = cbvIdxQueue.front();
	cbvIdxQueue.pop();

	return idx;
}

void CObjectPoolManager::ReturnCBVIndex(UINT idx)
{
	if (idx < 0) {
		return;
	}
	cbvIdxQueue.push(idx);
}

void CObjectPoolManager::ClearPool(const std::wstring& key)
{

}

void CObjectPoolManager::ClearPools()
{

}
