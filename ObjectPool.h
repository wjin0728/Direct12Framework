#pragma once
#include"stdafx.h"

class CGameObject;

class CObjectPool
{
private:
	std::wstring mName{};
	std::unique_ptr<CGameObject> mOriginal{};
	std::stack<std::shared_ptr<CGameObject>> mPoolStack{};

	UINT mObjectCnt{};

public:
	void Initialize(std::unique_ptr<CGameObject>&& original, UINT objCnt);

private:
	void CreatePoolableObject();

};

//오늘 할 일
//1. 오브젝트 풀 구현하기
//3. 오브젝트의 cbv 인덱스를 어떤 방식으로 지정해줄지 정하기