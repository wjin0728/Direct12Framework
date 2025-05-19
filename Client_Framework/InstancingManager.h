#pragma once
#include"stdafx.h"

struct 

class CInstancingManager
{
	MAKE_SINGLETON(CInstancingManager)

private:
	std::unordered_map<UINT, std::shared_ptr<CInstancingGroup>> mInstancingGroupMap{};
};

