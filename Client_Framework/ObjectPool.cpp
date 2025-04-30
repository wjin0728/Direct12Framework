#include "stdafx.h"
#include "ObjectPool.h"
#include"GameObject.h"
#include"Transform.h"

void CObjectPool::Initialize(std::unique_ptr<CGameObject>&& original, UINT objCnt)
{
	mOriginal = std::move(original);
	mObjectCnt = objCnt;
	mName = mOriginal->GetTag();

	for (UINT i = 0; i < mObjectCnt; i++) {
		PushObject(CreatePoolableObject());
	}
}

std::shared_ptr<CGameObject> CObjectPool::CreatePoolableObject() const
{
	if (!mOriginal) {
		return nullptr;
	}

	auto obj = CGameObject::Instantiate(mOriginal);
	obj->SetActive(false);

	return obj;
}

void CObjectPool::PushObject(std::shared_ptr<CGameObject> object)
{
	if (!object) {
		return;
	}
	object->ReturnCBVIndex();
	object->GetTransform()->Reset();
	object->SetActive(false);

	mPoolStack.push(object);
}

std::shared_ptr<CGameObject> CObjectPool::PopObject(const std::shared_ptr<CTransform>& parent)
{
	std::shared_ptr<CGameObject> popObject{};

	if (!mPoolStack.empty()) {
		popObject = mPoolStack.top();
		mPoolStack.pop();
	}
	else {
		popObject = CreatePoolableObject();
	}

	popObject->SetActive(true);

	if (parent) {
		popObject->GetTransform()->SetParent(parent);
	}

	return popObject;
}

void CObjectPool::Clear()
{

}
