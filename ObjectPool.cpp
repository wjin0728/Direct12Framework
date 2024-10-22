#include "stdafx.h"
#include "ObjectPool.h"
#include"GameObject.h"

void CObjectPool::Initialize(std::unique_ptr<CGameObject>&& original, UINT objCnt)
{
	mOriginal = std::move(original);
	mObjectCnt = objCnt;

}

void CObjectPool::CreatePoolableObject()
{
	for (UINT i = 0; i < mObjectCnt; i++) {
		auto obj = CGameObject::Instantiate(mOriginal);
		obj->SetActive(false);

		mPoolStack.push(obj);
	}
}
