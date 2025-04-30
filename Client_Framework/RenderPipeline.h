#pragma once
#include "stdafx.h"

class CRenderPipeline
{
private:
	std::weak_ptr<class CScene> mScene{};


public:
	CRenderPipeline() = default;
	~CRenderPipeline() = default;
};

