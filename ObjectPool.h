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

//���� �� ��
//1. ������Ʈ Ǯ �����ϱ�
//3. ������Ʈ�� cbv �ε����� � ������� ���������� ���ϱ�