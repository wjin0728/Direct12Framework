#pragma once
#include"stdafx.h"

class CGameObject;
class CTransform;

class CObjectPool
{
private:
	std::string mName{};
	std::unique_ptr<CGameObject> mOriginal{};
	std::stack<std::shared_ptr<CGameObject>> mPoolStack{};

	UINT mObjectCnt{};

public:
	void Initialize(std::unique_ptr<CGameObject>&& original, UINT objCnt);

private:
	std::shared_ptr<CGameObject> CreatePoolableObject() const;

private:
	friend class CObjectPoolManager;

	void PushObject(std::shared_ptr<CGameObject> object);
	std::shared_ptr<CGameObject> PopObject(const std::shared_ptr<CTransform>& parent = nullptr);

	void Clear();
};
