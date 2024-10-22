#pragma once
#include"stdafx.h"

class CGameObject;

class CObjectPoolManager
{
	MAKE_SINGLETON(CObjectPoolManager)

private:
	struct Pool
	{
		std::queue<std::shared_ptr<CGameObject>> poolQueue{};
	};

public:
	void Initialize();
};

